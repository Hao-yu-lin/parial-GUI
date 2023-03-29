import random

import numpy as np
import torch
from config import parse_args
from shadow import ShadowRemoval
import cv2


def main(addr):
    # print("sucess");
    args = parse_args()
    args.img_path = addr

    # image = cv2.imread(addr);
    # image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)
    
    
    same_seeds(args.seed)
    if args is None:
        exit()

    removal_model = ShadowRemoval(args)
    removal_model.build_model()
    image = removal_model.train()

    return image


def same_seeds(seed):
    # Python built-in random module
    random.seed(seed)
    # Numpy
    np.random.seed(seed)
    # Torch
    torch.manual_seed(seed)
    if torch.cuda.is_available():
        torch.cuda.manual_seed(seed)
        torch.cuda.manual_seed_all(seed)
    torch.backends.cudnn.benchmark = False
    torch.backends.cudnn.deterministic = True


if __name__ == '__main__':
    main()
