import argparse


def parse_args():
    desc = "Pytorch implementation of Shadowremoval"
    parser = argparse.ArgumentParser(description = desc)
    
    # path
    parser.add_argument('--img_path', type=str, default='./', help='test_path')
    parser.add_argument('--output_path', type=str, default='/home/haoyu/Desktop/GUI/parial-GUI/tmp_img')
    
    # ckpt
    # parser.add_argument('--removal_path', type=str, default='../parial-GUI/src/py_file/module_ckpt/shadow_genA2B.pt')
    # parser.add_argument('--class_path', type=str, default='../parial-GUI/src/py_file/module_ckpt/shadow_classifier_128.ckpt')
    # parser.add_argument('--removal_path', type=str, default='/home/haoyu/Desktop/GUI/parial-GUI/src/py_file/module_ckpt/shadow_genA2B.pt')
   
    # parser.add_argument('--removal_path', type=str, default='/home/haoyu/Desktop/GUI/parial-GUI/src/py_file/module_ckpt/05-07-10/train.pt') 
    parser.add_argument('--removal_path', type=str, default='../parial-GUI/src/py_file/module_ckpt/05-07-10/train.pt') 
    
    
    parser.add_argument('--img_size', type=int, default=64, help='The size of image')
    parser.add_argument('--device', type=str, default='cuda', choices=['cpu', 'cuda'], help='Set gpu mode; [cpu, cuda]')
    parser.add_argument('--seed', type=int, default=999)
    
    parser.add_argument('--batch_size', type=int, default=64, help='The batch size of image')

    args = parser.parse_args(args=[])
    
    return args

if __name__ == '__main__':
    args = parse_args()
    