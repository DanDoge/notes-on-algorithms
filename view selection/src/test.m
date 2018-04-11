function [] = test()
    home_path = 'C:\Users\AndrewHuang\Documents\GitHub\notes-on-algorithms\view selection\EXP_MODELS_LR';
    sub_path = dir(home_path);

    for i = 1 : length(sub_path)
        if( isequal(sub_path(i).name, '.') || ...
            isequal(sub_path(i).name, '..') || ...
            sub_path(i).isdir)
            continue;
        end
        naive_entropy(sub_path(i).name);
    end
end

function [] = naive_entropy(model_name)

    max_res = 0;
    max_path = 'nan';
    max_idx = 64;

    for i = 1:max_idx
        img_path = ['C:\Users\AndrewHuang\Documents\GitHub\notes-on-algorithms\view selection\res\' model_name '\' num2str(i) '.png'];
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
        if res > max_res
            max_res = res;
            max_path = img_path;
            fprintf('%d\n', i);
        end
    end
    copyfile(max_path, ['C:\Users\AndrewHuang\Documents\GitHub\notes-on-algorithms\view selection\res\' model_name  '\best_naive_entropy.png']);

end
