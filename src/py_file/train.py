import math
import os
import random

import cv2
import numpy as np
import torch
from config import parse_args
from models.network import Generator
from models.package.utils import RGB2BGR, denorm, tensor2numpy
from torch.utils.data import DataLoader, Dataset
from torchvision import transforms
from tqdm.auto import tqdm


@staticmethod
def img_preprocess(img):
    image_array = np.array(img, dtype=np.float32)
    r, g, b = np.split(image_array, 3, axis=2)
    # cv2.imwrite("/home/haoyu/Desktop/GUI/parial-GUI/tmp_img/test20.png", b, [cv2.IMWRITE_PNG_COMPRESSION, 9])

    r_mean = np.mean(r)
    g_mean = np.mean(g)
    b_mean = np.mean(b)
    
    r_mean = np.mean(r[r > r_mean])
    g_mean = np.mean(g[g > g_mean])
    b_mean = np.mean(b[b > b_mean])

    avg =  np.multiply(np.multiply(b_mean, g_mean), r_mean) ** (1.0/3)
    bCoef = avg/b_mean
    gCoef = avg/g_mean
    rCoef = avg/r_mean

    b = np.clip(b * bCoef, 0, 255)
    g = np.clip(g * gCoef, 0, 255)
    r = np.clip(r * rCoef, 0, 255)

    r_mean = np.mean(r)
    g_mean = np.mean(g)
    b_mean = np.mean(b)
    
    r_mean_new = np.mean(r[r > r_mean])
    g_mean_new = np.mean(g[g > g_mean])
    b_mean_new = np.mean(b[b > b_mean])
    
    new_coeff1 = 255 / min(r_mean_new, g_mean_new, b_mean_new)
    new_coeff2 = 255 / min(r_mean, g_mean, b_mean)

    new_coeff = math.sqrt(new_coeff1 * new_coeff2)
    
    r = np.squeeze(r, axis = 2)
    g = np.squeeze(g, axis = 2)
    b = np.squeeze(b, axis = 2)
    
    normalized_value = 4 / (((g + 1) / 256 * 3 + 1))
    log_value = np.log(normalized_value + 0.3) / np.log(1.6) - 0.3
      
    # normalized_value = 4 / (((g + 1) / 256 * 2 + 1))
    # log_value = np.log(normalized_value + 0.3) / np.log(1.67) - 0.35

    # 對b、g、r通道進行修改
    b = np.power(new_coeff, log_value) * b
    g = np.power(new_coeff, log_value) * g
    r = np.power(new_coeff, log_value) * r
    
    image_array[:, :, 0] = r
    image_array[:, :, 1] = g
    image_array[:, :, 2] = b 

    image_array = np.clip(image_array, 0, 255)
    b = np.clip(b, 0, 255)
    
    new_b = b

    return image_array, new_b


class Mask_Dataset(Dataset):
    def __init__(self, img, contours):
        self.img = img
        self.contours = contours
        self.tfm = transforms.Compose([
            transforms.ToPILImage(),
            transforms.ToTensor(),
            transforms.Normalize(mean=0.5, std=0.5),
        ])
        self.img_0, self.img_1, _ = self.img.shape
        self.blank = np.zeros_like(img)
        
        
    def __len__(self):
        return len(self.contours)
    
    def __getitem__(self, index):
        self.blank.fill(0)
        x, y, w, h = cv2.boundingRect(self.contours[index])
        
        # if(w * h < 96 * 96):
        #     cv2.drawContours(self.blank, [self.contours[index]], 0, color=(255,255,255), thickness=-1, lineType=cv2.LINE_AA,)
        #     rec_w, rec_h = int(w * 1.2), int(h * 1.2)
        #     cv2.rectangle(self.blank, (x, y), (x+rec_w, y+rec_h), (255,255,255), -1)
        #     (x_c, y_c), radius_c = cv2.minEnclosingCircle(self.contours[index])
        #     center = (int(x_c), int(y_c))
        #     radius = int(radius_c * 1.2)
        #     cv2.circle(self.blank, center, radius, (255,255,255), -1)
        cv2.drawContours(self.blank, [self.contours[index]], 0, color=(255,255,255), thickness=-1, lineType=cv2.LINE_AA,)
        cx, cy = x + w//2, y + h//2
        
        xmin, xmax = cx-48, cx+48
        ymin, ymax = cy-48, cy+48
        
        xmin = max(0, xmin)
        xmax = min(self.img_1, xmax)
        ymin = max(0, ymin)
        ymax = min(self.img_0, ymax)
               
        patch = np.ones((96, 96, 3))
        patch *= 255
        patch = patch.astype(np.uint8)  # 轉換回 uint8 型態
        
        patch[0:ymax - ymin, 0:xmax - xmin] = self.img[ymin:ymax, xmin:xmax]
        patch = self.tfm(patch)
        
        blank_threshold = np.zeros((96, 96, 3))
        blank_threshold[0:ymax - ymin, 0:xmax - xmin] = self.blank[ymin:ymax, xmin:xmax]
        
        x_y_range = np.array([(ymin, ymax),(xmin, xmax)])
        
        return patch, blank_threshold, x_y_range

def shadow_removal(args):
    img_path = args.img_path
    removal_path = args.removal_path 
    device = args.device
    batch_size = args.batch_size
    
    # load module
    removal_model = Generator().to(device)
    # param = torch.load(removal_path)
        # load module
    param = torch.load(removal_path)
    removal_model.load_state_dict(param['genA2B'])
    
    # removal_model.load_state_dict(torch.load(removal_path))
    removal_model.eval()
    
    img = cv2.imread(img_path)
    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    img, blue = img_preprocess(img)
    # cv2.imwrite("/home/haoyu/Desktop/GUI/parial-GUI/tmp_img/blue2.png", blue, [cv2.IMWRITE_PNG_COMPRESSION, 9])
    

    img = np.uint8(img)
    
    # blue = img[:, :, 2]
    # blue_array = np.array(blue.flatten())
    # blue_mean = np.mean(blue_array)
    # # blue_median = np.median(blue_array)
    
    # # blue_sqrt = math.sqrt(blue_mean * blue_median)
    # # new_blue = np.mean(blue[blue < blue_mean])
    # # threshold_value = new_blue
    # threshold_value = blue_mean
    
    
    '''
    if blue < blue_value = 255
    else = 0
    '''
    blue_thresh = np.where(blue < 200, np.uint8(255), np.uint8(0))
    # cv2.imwrite("/home/haoyu/Desktop/GUI/parial-GUI/tmp_img/blue_thresh.png", blue_thresh, [cv2.IMWRITE_PNG_COMPRESSION, 9])
    
    '''
    pixel value = 255 is object
    pixel value = 0 is background
    '''   
    
    # print(threshold_value)
    contours, hierarchy = cv2.findContours(image=blue_thresh, mode = cv2.RETR_CCOMP, method = cv2.CHAIN_APPROX_SIMPLE)
    
    img_set = Mask_Dataset(img, contours)
    img_loader = DataLoader(img_set, batch_size=batch_size, shuffle=False, num_workers=8, pin_memory=True)
    
    for idx, batch in enumerate(tqdm(img_loader)):
        data_batch, threshold, patch_range = batch
        
        with torch.no_grad():
            outputs, _, _ = removal_model(data_batch.to(device))
            
        
        for i in range(len(outputs)):
            ymin, ymax = patch_range[i][0]
            xmin, xmax = patch_range[i][1]
            
            shf_img = tensor2numpy(denorm(outputs[i])) * 255  # 08:48
            # shf_img = img_preprocess(shf_img)
            
            shf_img = shf_img[0:ymax - ymin, 0:xmax - xmin]
            blank_threshold = threshold[i][0:ymax - ymin, 0:xmax - xmin]
            blank_threshold = np.array(blank_threshold)
           
            tmp_img = img[ymin:ymax, xmin:xmax].copy()
            img[ymin:ymax, xmin:xmax] = np.where(blank_threshold == 255, shf_img, tmp_img)
    # img = img / new_coeff
    img = img.astype(np.uint8)
    
    
    return img

def main(addr):
    args = parse_args()
    if args is None:
        exit()
        
    # setting args
    args.img_path = addr
    args.device = 'cuda' if torch.cuda.is_available() else 'cpu'
    # output_path = args.output_path
    same_seeds(args.seed)
    img = shadow_removal(args)
    return img


def same_seeds(seed):
    random.seed(seed)
    np.random.seed(seed)
    torch.manual_seed(seed)
    if torch.cuda.is_available():
        torch.cuda.manual_seed(seed)
        torch.cuda.manual_seed_all(seed)
    torch.backends.cudnn.benchmark = False
    torch.backends.cudnn.deterministic = True

if __name__ == '__main__':
    path = "/home/haoyu/Desktop/GUI/parial-GUI/tmp_img/義式磨豆機/EK43vsDitting/IMG_4157.JPG"
    img = main(path)
    img = cv2.cvtColor(img, cv2.COLOR_RGB2BGR)
    cv2.imwrite("/home/haoyu/Desktop/GUI/parial-GUI/tmp_img/義式磨豆機/EK43vsDitting/IMG_4157.png", img, [cv2.IMWRITE_PNG_COMPRESSION, 9])
    # outputpath = "/home/haoyu/Desktop/GUI/parial-GUI/tmp_img/600dpi/shf_output"
    # fname =  sorted([os.path.join(path, x) for x in os.listdir(path) if (x.endswith(".png") or x.endswith(".JPG") or x.endswith(".jpg"))])
    # for idx, name in enumerate(fname):
   
    #     img = main(name)

    #     name = name.split("/")[-1].split(".")[0]
    #     img = cv2.cvtColor(img, cv2.COLOR_RGB2BGR)
    #     cv2.imwrite(os.path.join(outputpath, f"{name}.png"), img, [cv2.IMWRITE_PNG_COMPRESSION, 9])
    # cv2.imwrite(path, img, [cv2.IMWRITE_JPEG_QUALITY, 100])
    
    # path = "/home/haoyu/Desktop/GUI/parial-GUI/tmp_img/IMG_4012.JPG"
    # img = main(path)
    # img = cv2.cvtColor(img, cv2.COLOR_RGB2BGR)
    # cv2.imwrite("/home/haoyu/Desktop/GUI/parial-GUI/tmp_img/IMG_4012_shf.png", img, [cv2.IMWRITE_PNG_COMPRESSION, 9])
    