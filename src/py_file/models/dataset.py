import os
from PIL import Image, ImageOps
from torch.utils.data import Dataset
from torchvision import transforms

Image.MAX_IMAGE_PIXELS = None

    
class CreatDataset(Dataset):
    def __init__(self, path, img_size=128):
        self.image_path = path
        self.img_size = img_size
        self.img_files = self.__split__()
        self.tfm = transforms.Compose([
            transforms.ToTensor(),
            transforms.Normalize(mean=0.5, std=0.5),
        ])
        
    def __split__(self):
        img = Image.open(self.image_path).convert('RGB')
        origin_h, origin_w = img.size
        self.origin_size = (origin_h, origin_w)
    
        n_h = (int(origin_h/self.img_size) if origin_h % self.img_size == 0 else int(origin_h/self.img_size) + 1)
        n_w = (int(origin_w/self.img_size) if origin_w % self.img_size == 0 else int(origin_w/self.img_size) + 1)


        h = n_h * self.img_size - origin_h
        w = n_w * self.img_size - origin_w

        new_img = ImageOps.expand(img, (0, 0, w, h), fill="white")

        patches = []

        for i in range(n_w):
            for j in range(n_h):
                x = j * self.img_size
                y = i * self.img_size
                
                patch = new_img.crop((x, y, x + self.img_size, y + self.img_size))
                patches.append(patch)
        return patches

        
    def __len__(self):
        return len(self.img_files)
    
    def __imgsize__(self):
        return self.origin_size
    
    def __getitem__(self, index):
        img = self.img_files[index]
        img = self.tfm(img)
        
                
        return img
    
