import torch
import torch.nn as nn
from torch.nn.parameter import Parameter
from .package.library import *
# from package.library import *



# ngf：number of filters in the generator
# ndf：number of filters in the discriminator
class Generator(nn.Module):
    def __init__(self, input_c = 3,
                output_c = 3,
                n_blocks = [4, 6, 6, 8],
                img_size = 128,
                heads = [1,2,4,8],
                embed_dims=[32, 64, 128, 256],
                sr_ratios = [8, 4, 2, 1]):
        super(Generator, self).__init__()
        self.input_c = input_c
        self.output_c = output_c
        self.n_blocks = n_blocks
        self.len_blocks = len(n_blocks)
        self.sr_ratios = sr_ratios
        
        # patch_embed
        self.patch_embed_1 = OverlapPatchEmbed(img_size=img_size, patch_size=3, stride=2, in_chans=input_c,
                                              embed_dim=embed_dims[0])
        self.patch_embed_2 = OverlapPatchEmbed(img_size=img_size // 4, patch_size=3, stride=2, in_chans=embed_dims[0],
                                              embed_dim=embed_dims[1])
        self.patch_embed_3 = OverlapPatchEmbed(img_size=img_size // 8, patch_size=3, stride=2, in_chans=embed_dims[1],
                                              embed_dim=embed_dims[2])
        self.patch_embed_4 = OverlapPatchEmbed(img_size=img_size // 16, patch_size=3, stride=2, in_chans=embed_dims[2],
                                              embed_dim=embed_dims[3])
        
        for i in range(self.len_blocks):
            setattr(self, 'transformblock_' + str(i+1), nn.Sequential(*[TransformerBlock(dim = embed_dims[i], num_heads=heads[i], sr_ratio=self.sr_ratios[i]) for j in range(self.n_blocks[i])]))
        
        for i in range(self.len_blocks):
            setattr(self, 'resnetblock_' + str(i+1), nn.Sequential(*[ResnetBlock(dim=embed_dims[i]) for j in range(self.n_blocks[i])]))

        for i in range(self.len_blocks):
            setattr(self, 'GAPandGMP_' + str(i+1), GAPAndGMP(dim=embed_dims[i]))
        
        for i in range(self.len_blocks):
            setattr(self, 'GammaBeta_' + str(i+1), GammaBeta(dim=embed_dims[i]))
            
        for i in range(self.len_blocks):
            setattr(self, 'resnetadainblock_' + str(i+1), ResnetAdaILNBlock(dim=embed_dims[i]))

        for i in range(self.len_blocks):
            setattr(self, 'upsampling_' + str(i+1), UpSampling(dim=embed_dims[i], state="bilinear"))    # 'linear', 'bilinear', 'bicubic', or 'trilinear'.
            
        self.output_layer = nn.Sequential(
            nn.ReflectionPad2d(3),
            nn.Conv2d(embed_dims[0]//2, output_c, kernel_size=7, stride=1, padding=0, bias=False)
        )
        
        self.prewitt_x = nn.Conv2d(output_c, output_c, 3, stride=1, padding=1, bias = False)
        sobel_kernel_x = torch.FloatTensor([[3, 10, 3], [0, 0, 0], [-3, -10, -3]])
        sobel_kernel_x = nn.Parameter(sobel_kernel_x.unsqueeze(0).unsqueeze(0), requires_grad=False)
        self.prewitt_x.weight.data = sobel_kernel_x.repeat(output_c, output_c, 1, 1)
        
        self.prewitt_y = nn.Conv2d(output_c, output_c, 3, stride=1, padding=1, bias = False)
        sobel_kernel_y = torch.FloatTensor([[3, 0, -3], [10, 0, -10], [3, 0, -3]])
        sobel_kernel_y = nn.Parameter(sobel_kernel_y.unsqueeze(0).unsqueeze(0), requires_grad=False)
        self.prewitt_y.weight.data = sobel_kernel_y.repeat(output_c, output_c, 1, 1)
        
        self.prewitt_45 = nn.Conv2d(output_c, output_c, 3, stride=1, padding=1, bias = False)
        sobel_kernel_45 = torch.FloatTensor([[0, -3, -10], [3, 0, -3], [10, 3, 0]])
        sobel_kernel_45 = nn.Parameter(sobel_kernel_45.unsqueeze(0).unsqueeze(0), requires_grad=False)
        self.prewitt_45.weight.data = sobel_kernel_45.repeat(output_c, output_c, 1, 1)
        
        self.prewitt_135 = nn.Conv2d(output_c, output_c, 3, stride=1, padding=1, bias = False)
        sobel_kernel_135 = torch.FloatTensor([[10, 3, 0], [3, 0, -3], [0, -3, -10]])
        sobel_kernel_135 = nn.Parameter(sobel_kernel_135.unsqueeze(0).unsqueeze(0), requires_grad=False)
        self.prewitt_135.weight.data = sobel_kernel_135.repeat(output_c, output_c, 1, 1)
        
        
        self.norm = nn.InstanceNorm2d(output_c) 
        self.relu = nn.ReLU(True)
    def forward(self, x):
        output1_1 = self.patch_embed_1(x)
        output1_ = self.transformblock_1(output1_1)
        
        output2_1 = self.patch_embed_2(output1_)
        output2_ = self.transformblock_2(output2_1)
        
        output3_1 = self.patch_embed_3(output2_)
        output3_ = self.transformblock_3(output3_1)
        
        output4_1 = self.patch_embed_4(output3_)
        output4_ = self.transformblock_4(output4_1)
        
        # B x embed_dims[3] x H/16 x W/16
        output4 = self.resnetblock_4(output4_)
        output4 = output4 + output4_1 
        output4 = self.upsampling_4(output4)    #  B x embed_dims[2] x H/8 x W/8
        
        # B x embed_dims[2] x H/8 x W/8
        output3 = self.resnetblock_3(output4)
        output3 = output3 + output3_1
        output3 = self.upsampling_3(output3)    # B x embed_dims[1] x H/4 x W/4
        
        # B x embed_dims[1] x H/4 x W/4
        output2 = self.resnetblock_2(output3)
        output2 = output2 + output2_1
        output2 = self.upsampling_2(output2)    # B x embed_dims[0] x H/2 x W/2
        
        # B x embed_dims[0] x H/2 x W/2
        output1 = self.resnetblock_1(output2)
        output1 = output1 + output1_1
        output1 = self.upsampling_1(output1)
        output = self.output_layer(output1) + x
        
        pre_x = self.relu(self.norm(self.prewitt_x(output)))
        pre_y = self.relu(self.norm(self.prewitt_y(output)))
        pre_45 = self.relu(self.norm(self.prewitt_45(output)))
        pre_135 = self.relu(self.norm(self.prewitt_135(output)))
        
        pre = 0.25 * pre_x + 0.25 * pre_y + 0.25 * pre_45 + 0.25 * pre_135
        
        output = (0.7 * pre + output).tanh()

        return output, -1, -1
        

class Discriminator(nn.Module):
    def __init__(self, input_c=3, ndf=64, n_layers=5):
        super(Discriminator, self).__init__()
        self.n_layers = n_layers
        self.initial = nn.Sequential(
            nn.utils.spectral_norm(nn.Conv2d(input_c, ndf, kernel_size=4, stride=2, padding=1, bias=False)),
            # nn.LeakyReLU(0.2, True)
            nn.ReLU(True),
        )
        
        for i in range(n_layers - 1):
            mult = 2**i
            setattr(self, 'disblock_' + str(i+1), DisBlock(ndf * mult, ndf * mult * 2))
        
        mult = 2 ** (n_layers - 1)
        self.gapandgmp = GAPAndGMP(ndf*mult)
        self.output = nn.Conv2d(ndf*mult, 1, kernel_size=3, stride=1, padding=1, bias=False)
        
    def forward(self, x):
        x = self.initial(x)
        for i in range(self.n_layers-1):
            x = getattr(self, 'disblock_'+ str(i+1))(x)
            # print(x.shape)
        x, cam_logit, heatmap = self.gapandgmp(x)
        out = self.output(x)
        # print(out.shape)
        return out, cam_logit, heatmap
    

class RhoClipper(object):

    def __init__(self, min, max):
        self.clip_min = min
        self.clip_max = max
        assert min < max

    def __call__(self, module):

        if hasattr(module, 'rho'):
            w = module.rho.data
            w = w.clamp(self.clip_min, self.clip_max)
            module.rho.data = w
            
        
if __name__ == '__main__':
    img = torch.rand(1, 3, 128, 128)
    # gen = Generator()
    dis = Discriminator(n_layers=5)
    dis(img)
    # gap_and_gmp = GAPAndGMP(32)
    
    # gap_and_gmp(img)
 

        