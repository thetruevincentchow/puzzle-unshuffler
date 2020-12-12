import warnings

import os
import sys
import numpy as np
from PIL import Image, ImageCms

srgb_profile = ImageCms.createProfile("sRGB")
lab_profile  = ImageCms.createProfile("LAB")

rgb2lab_transform = ImageCms.buildTransformFromOpenProfiles(srgb_profile, lab_profile, "RGB", "LAB")

fp = os.fdopen(sys.stdout.fileno(), 'wb')

newin = sys.stdin

while True:
    try:
        line = sys.stdin.readline()
    except KeyboardInterrupt:
        break
    if not line: break

    name = line.strip()

    #print("Python",name, file=sys.stderr)
    img = Image.open(name)
    lab_img = ImageCms.applyTransform(img, rgb2lab_transform)
    data = np.array(lab_img)

    #print(len(bytes(data.flatten())))
    fp.write(bytes(data.flatten()))
    fp.flush()

