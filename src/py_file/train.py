import random
import cv2
import numpy as np
import torch

from models.network import Generator
from config import parse_args
from torchvision import transforms
from tqdm.auto import tqdm
from torch.utils.data import Dataset, DataLoader
from models.package.utils import denorm, RGB2BGR, tensor2numpy

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
        # print(self.blank.shape)
        cv2.drawContours(self.blank, [self.contours[index]], 0, color=(255,255,255), thickness=-1, lineType=cv2.LINE_AA,)
        x, y, w, h = cv2.boundingRect(self.contours[index])
        
        cx, cy = x + w//2, y + h//2
        
        xmin, xmax = cx-32, cx+32
        ymin, ymax = cy-32, cy+32
        
        xmin = max(0, xmin)
        xmax = min(self.img_1, xmax)
        ymin = max(0, ymin)
        ymax = min(self.img_0, ymax)
               
        patch = np.ones((64, 64, 3))
        patch *= 255
        patch = patch.astype(np.uint8)  # 轉換回 uint8 型態
        
        patch[0:ymax - ymin, 0:xmax - xmin] = self.img[ymin:ymax, xmin:xmax]
        patch = self.tfm(patch)
        
        blank_threshold = np.zeros((64, 64, 3))
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
    removal_model.load_state_dict(torch.load(removal_path))
    removal_model.eval()
    
    
    img = cv2.imread(img_path)
    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    blue = img[:, :, 2]
    
    '''
    if blue < blue_value = 255
    else = 0
    '''
    blue_thresh = np.where(blue < 200, np.uint8(255), np.uint8(0))
    
    '''
    pixel value = 255 is object
    pixel value = 0 is background
    '''   
    contours, _ = cv2.findContours(image=blue_thresh, mode = cv2.RETR_EXTERNAL, method = cv2.CHAIN_APPROX_SIMPLE)
    
    img_set = Mask_Dataset(img, contours)
    img_loader = DataLoader(img_set, batch_size=batch_size, shuffle=False, num_workers=8, pin_memory=True)
    
    for idx, batch in enumerate(tqdm(img_loader)):
        data_batch, threshold, patch_range = batch
        
        with torch.no_grad():
            outputs, _, _ = removal_model(data_batch.to(device))
            
        # print(outputs.shape)        # b, c, 64, 64
        # print(threshold.shape)      # b, 64, 64, 3
        # print(patch_range.shape)    # b, 2, 2
        # print(len(outputs))
        
        for i in range(len(outputs)):
            ymin, ymax = patch_range[i][0]
            xmin, xmax = patch_range[i][1]
            
            shf_img = tensor2numpy(denorm(outputs[i])) * 255  # 08:48
            
            shf_img = shf_img[0:ymax - ymin, 0:xmax - xmin]
            blank_threshold = threshold[i][0:ymax - ymin, 0:xmax - xmin]
            blank_threshold = np.array(blank_threshold)
           
            tmp_img = img[ymin:ymax, xmin:xmax].copy()
            img[ymin:ymax, xmin:xmax] = np.where(blank_threshold == 255, shf_img, tmp_img)
        
    img = img.astype(np.uint8)
        
    # img = cv2.cvtColor(img, cv2.COLOR_RGB2BGR)
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
    img_path = "/home/haoyu/Desktop/GUI/parial-GUI/tmp_img/A1-390_600dpi-co.png"
    img = main(img_path)
    
    # cv2.imwrite(output_path+"/test10.png", img, [cv2.IMWRITE_PNG_COMPRESSION, 5])
    # cv2.imwrite(path, img, [cv2.IMWRITE_JPEG_QUALITY, 100])