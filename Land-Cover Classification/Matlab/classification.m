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

%% Initialize Variables

% empty array
r_arr = ([]);
g_arr = ([]);
b_arr = ([]);
fe_arr =([]);
le_arr = ([]);
nir_arr = ([]);

% loop for 4 classes
for k = 1:size(stats,1)
    
    % loop for each co-ordinates, per class from each images
    for i = 1:size(stats(k).PixelList,1)
        r_arr(i,1) = r_img(stats(k).PixelList(i,2), stats(k).PixelList(i,1));
        g_arr(i,1) = g_img(stats(k).PixelList(i,2), stats(k).PixelList(i,1));
        b_arr(i,1) = b_img(stats(k).PixelList(i,2), stats(k).PixelList(i,1));
        fe_arr(i,1) = fe_img(stats(k).PixelList(i,2), stats(k).PixelList(i,1));
        le_arr(i,1) = le_img(stats(k).PixelList(i,2), stats(k).PixelList(i,1));
        nir_arr(i,1) = nir_img(stats(k).PixelList(i,2), stats(k).PixelList(i,1));
    end
    
    % empty cell
    SampleCell = {};
    covCell = {};
    
    % pixel per sample set 
      pixelSize = 1;
    
    % total 30 samples per class, 5 samples from each image
    for i = 1:5 
        SampleCell(i,1,k) = {datasample(r_arr,pixelSize)}; 
        SampleCell(i,2,k) = {datasample(g_arr,pixelSize)};
        SampleCell(i,3,k) = {datasample(b_arr,pixelSize)};
        SampleCell(i,4,k) = {datasample(fe_arr,pixelSize)};
        SampleCell(i,5,k) = {datasample(le_arr,pixelSize)};
        SampleCell(i,6,k) = {datasample(nir_arr,pixelSize)};
    end
    
    for j = 1:6
           muArray(k,j) = mean(cell2mat(SampleCell(:,j,k)));
    end
    % covariance matrix is 6X6 for each class
    covCell(k,:) = {cov(cell2mat(SampleCell(:,:,k)))}; 
 end
 
 % % validation with training data 
   confusionTrainMatrix = ([]);
   n = 30;

   for cl = 1:4 %4 class 
    
        class1cnt = 0;
        class2cnt = 0;
        class3cnt = 0;
        class4cnt = 0;
        
        for feature = 1:size(cell2mat(SampleCell(:,:,cl)),2) % each feature 6
            pixel = cell2mat(SampleCell(:,feature,cl));
        end
   end
