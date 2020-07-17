clear all; close all; clc;
pkg load image;

%converts an image to BGR565 arduino array 

%input image 
imageName = "pine.png"

%name for the image variable 
arrayName = "background"


I = imread (imageName); % RGB image
set(gcf,'Name','Input Image'); 
imshow(I);

textfile = "C array output 1.txt";
fid = fopen (textfile, 'wt'); % generates text file in write mode

for i = 1:3
    IN(:,:,i) = I(:,:,i);

end
r = IN(:,:,1);
g = IN(:,:,2);
b = IN(:,:,3);

imageinfo = imfinfo(imageName);

 fprintf(fid, '//\"%s\" width=%u height=%u\n', imageName, imageinfo.Width, imageinfo.Height);

 fprintf(fid, 'const uint16_t %s [] PROGMEM = {', arrayName);


for y = 1:  imageinfo.Height
  y
  for x = 1: imageinfo.Width
    
    rvalue = uint16(bitshift(r(y,x), - 3));
    gvalue = uint16(bitshift(g(y,x), - 2));
    bvalue = uint16(bitshift(b(y,x), - 3));
    
    val = 0; 
    val = uint16(bitshift(bitor(val, bvalue), 6));
    val = uint16(bitshift(bitor(val, gvalue), 5));
    val = uint16(bitor(val, rvalue));
      fprintf(fid, "0x%s", dec2hex(val,4));
    if ~(x == imageinfo.Width && y == imageinfo.Height)
      fprintf(fid, ", ");
      end;

  end;  
   fprintf(fid, '\n');
  end;

   fprintf(fid, '};');

fclose (fid);