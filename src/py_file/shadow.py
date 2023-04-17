import torch
import os
from torch.utils.data import DataLoader
from models.network import Generator
from datetime import datetime
from models.class_module import Classifier
from models.package.utils import denorm, tensor2numpy, RGB2BGR, combine_patches, save_shadowimg
from models.dataset import CreatDataset
from tqdm.auto import tqdm
import numpy as np
import cv2

class ShadowRemoval(object):
    def __init__(self, args):
        self.img_path = args.img_path
        exp_time = datetime.today().strftime("%m-%d-%H")
        self.output_path = os.path.join(args.output_path, exp_time)
        os.makedirs(self.output_path, exist_ok=True)
        self.img_size = args.img_size
        self.device = args.device
        self.removal_path = args.removal_path
        self.class_path = args.class_path
        
    
    def build_model(self):
        self.img_set = CreatDataset(self.img_path)
        self.img_loader = DataLoader(self.img_set, batch_size=1, shuffle=False, num_workers=2, pin_memory=True)
        self.removal_model = Generator().to(self.device)
        self.classifier_model = Classifier(img_size=self.img_size).to(self.device)
        self.origin_size = self.img_set.__imgsize__()
        
    def train(self):
        param = torch.load(self.removal_path)
        # load module
        self.removal_model.load_state_dict(param['genA2B'])
        print("ok1")
        self.removal_model.eval()
        
        self.classifier_model.load_state_dict(torch.load(self.class_path))
        print("ok2")
        self.classifier_model.eval()
        
        patches = []
        for imgs in tqdm(self.img_loader):
            imgs = imgs.to(self.device)
            b, _, _, _ = imgs.shape

            with torch.no_grad():
                # outputs, _, _ = model(imgs.to(device))
                logitces = self.classifier_model(imgs)
                if(logitces > 0.5):
                    outputs, _, _ = self.removal_model(imgs)
                    # outputs = outputs[0]
                else:
                    outputs = imgs
            for i in range(b):
                # tmp_img = RGB2BGR(tensor2numpy(denorm(outputs[i])))
                tmp_img = tensor2numpy(denorm(outputs[i]))
                
                patches.append(tmp_img * 255.0)
        
        concat_img = combine_patches(patches, self.origin_size, img_size=self.img_size)
        # print(concat_img.shape)
        # save_shadowimg(path=self.output_path+"/test4.jpeg", img=concat_img)
        return concat_img.astype(np.uint8);  
        

        