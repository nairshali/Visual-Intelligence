%% Clear Window
clear all; close all; clc;

%% Read Images
load('C:\LandCoverImages\ground_truth.mat');
r_img = imread("C:\LandCoverImages\r.dib");
g_img = imread("C:\LandCoverImages\g.dib");
b_img = imread("C:\LandCoverImages\b.dib");
fe_img = imread("C:\LandCoverImages\fe.dib");
le_img = imread("C:\LandCoverImages\le.dib");
nir_img = imread("C:\LandCoverImages\nir.dib");

%% coordinates for each class
stats = regionprops(labelled_ground_truth, 'PixelList');

% loop for 4 classes
for k = 1:4
    
    % loop for each co-ordinates, per class from each images
    for i = 1:3
        r_arr(i,1) = r_img(stats(k).PixelList(i,2), stats(k).PixelList(i,1));
        g_arr(i,1) = g_img(stats(k).PixelList(i,2), stats(k).PixelList(i,1));
        b_arr(i,1) = b_img(stats(k).PixelList(i,2), stats(k).PixelList(i,1));
        fe_arr(i,1) = fe_img(stats(k).PixelList(i,2), stats(k).PixelList(i,1));
        le_arr(i,1) = le_img(stats(k).PixelList(i,2), stats(k).PixelList(i,1));
        nir_arr(i,1) = nir_img(stats(k).PixelList(i,2), stats(k).PixelList(i,1));
    end
    
 end
