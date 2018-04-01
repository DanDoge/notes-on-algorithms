function [] = silouette_entropy()
    max_res = 0;
    max_path = 'nan';
    max_idx = 216;

    for i = 1:max_idx
        img_path = ['C:\Users\AndrewHuang\Documents\GitHub\notes-on-algorithms\view selection\res\' num2str(i) '.png'];
        img = imread(img_path);
        img_gray = rgb2gray(img);
        img_bin = im2bw(img_gray, 0.9);
        [b, ~] =  bwboundaries(~img_bin,'noholes');
        contour = b{1, 1};
        plot(contour(:, 1), contour(:, 2));
        res = get_entropy(contour);
        if res > max_res
            max_res = res;
            max_path = img_path;
            fprintf('%d\n', i);
        end
    end
end

function res = get_entropy(contour)
    min_dis = 114514;
    size = length(contour);
    for idx = 1: size
        idx1 = idx;
        idx2 = idx1 + 1;
        if idx1 == size
            idx2 = 1;
        end
        min_dis = min(min_dis, sqrt((contour(idx1, 1) - contour(idx2, 1))^2 + (contour(idx1, 2) - contour(idx2, 2))^2));
    end

    cnt = zeros(360);
    sum = 0;

    %for idx = 1: size
    %    idx1 = idx;
    %    if idx1 == size
    %        idx2 = 1;
    %    end
    %    tmp = floor(sqrt((contour(idx1, 1) - contour(idx2, 1))^2 + (contour(idx1, 2) - contour(idx2, 2))^2) / min_dis);
    %    cnt(1) = cnt(1) + tmp;
    %    sum = sum + tmp;
    %end

    for idx = 1: size - 2
        idx1 = idx;
        idx2 = idx + 1;
        idx3 = idx + 2;
        if idx2 > size
            idx2 = idx2 - size;
        end
        if idx3 > size
            idx3 = idx3 - size;
        end
        x1 = contour(idx1, 1);
        x2 = contour(idx2, 1);
        x3 = contour(idx3, 1);
        y1 = contour(idx1, 2);
        y2 = contour(idx2, 2);
        y3 = contour(idx3, 2);
        theta = acosd(dot([x1 - x2, y1 - y2], [x3 - x2, y3 - y2]) / (norm([x1 - x2, y1 - y2]) * norm([x3 - x2, y3 - y2])));
        %fprintf('%d %d %d\n', theta, idx, size);
        %fprintf('%d %d %d %d %d %d\n', x1, x2, x3, y1, y2, y3);
        if theta ~= NaN
            cnt(round(theta) + 181) = cnt(round(theta) + 181) + 1;
            sum = sum + 1;
        end
    end

    res = 0;
    for idx = 1: size
        cnt(idx) = cnt(idx) / sum;
        if cnt(idx) ~= 0.0
            res = res - cnt(idx) * log(cnt(idx)) / log(2.0);
        end
    end

end
