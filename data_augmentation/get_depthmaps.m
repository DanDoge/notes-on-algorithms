function [] = get_depthmaps()
    path = 'C:\Users\AndrewHuang\Documents\GitHub\notes-on-algorithms\data_augmentation\models\';
    sub_path = dir(path);

    for i = 1 : length(sub_path)
        if( isequal(sub_path(i).name, '.') || ...
            isequal(sub_path(i).name, '..') || ...
            sub_path(i).isdir)
            continue;
        end
        %get_single_depthmap(i, path, sub_path(i).name);
    end
    get_single_depthmap(1, path, 'chair2.obj')
end

function [] = get_single_depthmap(idx, path, obj_name)
    [vertices, faces] = obj__read([path obj_name]);
    FV.vertice = (vertices)';
    FV.faces = (faces)';
    if idx == 1
        clf, patch(FV, 'facecolor', [0.5, 0.5, 0.5], 'linestyle', 'none'), axis equal, axis off, axis image, hidden on, camlight('headlight', 'infinite');
    else
        clf, patch(FV, 'facecolor', [0.5, 0.5, 0.5]), axis equal, axis off, axis image, hidden on, camlight('headlight', 'infinite');
    end
    
    for az = 0 : 60 : 300
        for el = -60 : 40 : 60
            view(az, el)
            f = getframe(gcf);
            img = rgb2gray(f.cdata);
            img_small = imresize(img, 0.1);
            path_tmp = [obj_name '_', mat2str(az), '_', mat2str(el), '.jpg'];
            imwrite(img_small, path_tmp);
        end
    end
end
