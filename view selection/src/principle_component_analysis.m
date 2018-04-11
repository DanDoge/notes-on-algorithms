function [] = principle_component_analysis()

    obj = read_wobj('C:\Users\AndrewHuang\Documents\GitHub\notes-on-algorithms\view selection\EXP_MODELS_LR\desk_chair_LR.obj');

    FV.vertice = obj.vertices;

    FV.faces = obj.objects(length(obj.objects)).data.vertices;

    figure, patch(FV, 'facecolor', [0.5, 0.5, 0.5]), axis equal, axis off, axis image, hidden on;

    [COEFF ,~ ,~] = pca(FV.vertice);

    % prime is the prin. axis, what about choose the prin. plane?
    prime = COEFF(:, 1);
    second = COEFF(:, 2);

    % B is the null space of tmp
    B = null([prime'; second']);
    view(B);


end
