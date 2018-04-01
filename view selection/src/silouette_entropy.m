function [] = silouette_entropy()
    max_res = 0;
    max_path = 'nan';

    for i = 1:216
        img_path = ['C:\Users\AndrewHuang\Documents\GitHub\notes-on-algorithms\view selection\res\' num2str(i) '.png'];
        img = imread(img_path);
        img_gray = rgb2gray(img);
        img_bin = im2bw(img_gray, 0.9);
        [b, l] =  bwboundaries(~img_bin,'noholes');
        contour = b{1, 1};
        plot(contour(:, 1), contour(:, 2));

    end
end

function [res] = get_entropy(contour)

end
