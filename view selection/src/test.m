function [] = test()
    home_path = 'C:\Users\AndrewHuang\Documents\GitHub\notes-on-algorithms\view selection\EXP_MODELS_LR';
    sub_path = dir(home_path);

    for i = 1 : length(sub_path)
        if( isequal(sub_path(i).name, '.') || ...
            isequal(sub_path(i).name, '..') || ...
            sub_path(i).isdir)
            continue;
        end
        principle_component_analysis(i, sub_path(i).name);
    end
end

function [] = principle_component_analysis(idx, obj_name)

    [vertices, faces] = obj__read(['C:\Users\AndrewHuang\Documents\GitHub\notes-on-algorithms\view selection\EXP_MODELS_LR\' obj_name]);

    FV.vertice = (vertices)';

    FV.faces = (faces)';

    if idx == 1
        figure, patch(FV, 'facecolor', [0.5, 0.5, 0.5]), axis equal, axis off, axis image, hidden on;
    else
        clf, patch(FV, 'facecolor', [0.5, 0.5, 0.5]), axis equal, axis off, axis image, hidden on;
    end

    [COEFF ,~ ,~] = pca(FV.vertice);

    % prime is the prin. axis, what about choose the prin. plane?
    prime = COEFF(:, 1);
    second = COEFF(:, 2);

    % B is the null space of tmp
    B = null([prime'; second']);
    view(B);
    saveas(gcf, ['C:\Users\AndrewHuang\Documents\GitHub\notes-on-algorithms\view selection\res\' obj_name '\best_pca.png']);
end
