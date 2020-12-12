"""
Simple utility to visualise unshuffled images.
"""

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.image as mpimg

from argparse import ArgumentParser

import subprocess

N = 512 # size of the image in pixels
K = 64  # size of each square block in pixels

if __name__ == "__main__":
    parser = ArgumentParser()
    parser.add_argument("source", help="Path of source image")

    args = parser.parse_args()
    source_path = args.source

    
    img = mpimg.imread(source_path)
    plt.subplot(1, 2, 1)
    imgplot = plt.imshow(img)

    data = subprocess.check_output(["./reconstruct", source_path])
    lines = data.rstrip().split(b"\n")
    assert len(lines) % 2 == 0

    perm_results = {lines[i].decode() : list(map(int, lines[i+1].strip().split(b" "))) for i in range(0, len(lines), 2)}

    perm = perm_results[source_path]

    img_channels = img.transpose((2,0,1))
    pieces = [img_channels[:, K*x : K*(x+1), K*y:K*(y+1)] for x in range(N//K) for y in range(N//K)]
    unshuffled_img = np.block([[pieces[perm[x * (N//K) + y]] for y in range(N//K)] for x in range(N//K)]).transpose((1, 2, 0))
    plt.subplot(1, 2, 2)
    imgplot = plt.imshow(unshuffled_img)

    plt.show()

