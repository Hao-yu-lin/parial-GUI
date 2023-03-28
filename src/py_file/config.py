import argparse

def parse_args():
    desc = "Pytorch implementation of Shadowremoval"
    parser = argparse.ArgumentParser(description = desc)
    
    # path
    parser.add_argument('--img_path', type=str, default='./', help='test_path')
    parser.add_argument('--output_path', type=str, default='/home/haoyu/Desktop/partical/MyProject/output')
    
    # ckpt
    parser.add_argument('--removal_path', type=str, default='/home/haoyu/Desktop/partical/shadow_removal3/output/ckpt/03-14-04/train.pt')
    parser.add_argument('--class_path', type=str, default='/home/haoyu/Desktop/partical/shadow_removal3/output/ckpt/03-12-12_128/best.ckpt')
    
    parser.add_argument('--img_size', type=int, default=128, help='The size of image')
    parser.add_argument('--device', type=str, default='cuda', choices=['cpu', 'cuda'], help='Set gpu mode; [cpu, cuda]')
    parser.add_argument('--seed', type=int, default=999)
    

    args = parser.parse_args(args=[])
    
    return args

if __name__ == '__main__':
    args = parse_args()
    