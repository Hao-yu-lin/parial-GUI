import random
import numpy as np
import torch
from shadow import ShadowRemoval
from config import parse_args


def main():
    print("sucess");
    #args = parse_args()
    #same_seeds(args.seed)
    #if args is None:
        #exit()
     
    #removal_model = ShadowRemoval(args)
    #removal_model.build_model()
    #removal_model.train()
        

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
    
    

    
