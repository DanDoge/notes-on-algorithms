function [] = principle_component_analysis()

    obj = read_wobj('C:\Users\AndrewHuang\Documents\GitHub\notes-on-algorithms\view selection\EXP_MODELS_LR\desk_chair_LR.obj');

    FV.vertice = obj.vertices;

    FV.faces = obj.objects(length(obj.objects)).data.vertices;

    figure, patch(FV, 'facecolor', [0.5, 0.5, 0.5]), axis equal, axis off, axis image, hidden on;

    [COEFF ,~ ,~] = pca(FV.vertice);

    % tmp is the prin. axis
    tmp = COEFF(:, 1);

    % B is the null space of tmp
    B = null(tmp');

end
