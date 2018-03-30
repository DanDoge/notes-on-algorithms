function [] = naive_entropy()

    max_res = 0;
    max_path = 'nan';

    for i = 1:216
        img_path = ['C:\Users\AndrewHuang\Documents\GitHub\notes-on-algorithms\view selection\res\' num2str(i) '.png'];
        img = imread(img_path);
        img_gray = rgb2gray(img);
        [row, col] = size(img_gray);

        cnt = zeros(256);
        for ii = 1: row
            for jj = 1:col
                cnt(img_gray(ii, jj) + 1) = cnt(img_gray(ii, jj) + 1) + 1;
            end
        end

        res = 0;
        for ii = 1:256
            cnt(ii) = cnt(ii) / (row * col);
            if cnt(ii) ~= 0.0
                res = res - cnt(ii) * log(cnt(ii)) / log(2.0);
            end
        end
        %fprintf('%d\n', res);
        if res > max_res
            max_res = res;
            max_path = img_path;
            fprintf('%d\n', i);
        end
    end


end
