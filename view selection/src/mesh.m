% Author: DanDoge
% Date  : 18-04-18
% not tested

function [] = mesh()
    [vertices, faces] = obj__read('C:\Users\AndrewHuang\Documents\GitHub\notes-on-algorithms\view selection\EXP_MODELS_LR\airplane3_LR.obj');
    vertex_size = length(vertices);
    face_size = length(faces);
    vertices = vertices';
    faces = faces';
    fprintf('%d vertices, %d faces loaded\n', vertex_size, face_size);

    min_x = 32767; min_y = 32767; min_z = 32767;
    max_x = -32767; max_y = -32767; max_z = -32767;
    for i = 1 : vertex_size
        min_x = min(vertices(i, 1), min_x);
        min_y = min(vertices(i, 2), min_y);
        min_z = min(vertices(i, 3), min_z);

        max_x = min(vertices(i, 1), max_x);
        max_y = min(vertices(i, 2), max_y);
        max_z = min(vertices(i, 3), max_z);

    end

    normals = zeros(vertex_size, 3);
    face_area = zeros(vertex_size, 1);
    for i = 1 : face_size
        v1 = vertices(faces(i, 1), :);
        v2 = vertices(faces(i, 2), :);
        v3 = vertices(faces(i, 3), :);

        cross_prod = cross(v1 - v2, v3 - v2);
        face_area(i) = norm(cross_prod);
        for j = 1 : 3
            normals(faces(i, j), :) = normals(faces(i, j), :) + cross_prod;
        end
    end
    for i = 1 : vertex_size
        normals(i, :) = normals(i, :) / norm(normals(i, :));
    end

    shape_operators = zeros(3, 3, vertex_size);
    vertex_area = zeros(vertex_size, 1);
    for i = 1 : face_size
        for j = 1 : 3

            jj = j + 1;
            if jj == 4
                jj = 1;
            end

            v1_idx = faces(i, j);
            v2_idx = faces(i, jj);
            v1 = vertices(v1_idx, :);
            v2 = vertices(v2_idx, :);
            normal1 = normals(v1_idx, :);
            normal2 = normals(v2_idx, :);

            T12 = (eye(3) - wedge(v1)) * ((v1 - v2)');
            T12 = T12 / norm(T12);
            kappa_ij = 2 * dot(normal1, v2 - v1);
            kappa_ij = kappa_ij / norm(v1 - v2);
            shape_operators(:, :, v1_idx) = shape_operators(:, :, v1_idx) + wedge(T12) * kappa_ij * face_area(i);
            vertex_area(v1_idx) = vertex_area(v1_idx) + face_area(i);

            T21 = (eye(3) - wedge(v2)) * ((v2 - v1)');
            T21 = T21 / norm(T21);
            kappa_ji = 2 * dot(normal2, v1 - v2);
            kappa_ji = kappa_ji / norm(v2 - v1);
            shape_operators(:, :, v2_idx) = shape_operators(:, :, v2_idx) + wedge(T21) * kappa_ji * face_area(i);
            vertex_area(v2_idx) = vertex_area(v2_idx) + face_area(i);
        end
    end

    for i = 1 : vertex_size
        shape_operators(:, :, i) = shape_operators(:, :, i) / vertex_area(i);
    end
    disp(shape_operators);

    mean_curvature = zeros(vertex_size, 1);
    max_mean_curvature = 0.0;
    min_mean_curvature = 12345678.0;
    for i = 1 : vertex_size
        one = [1, 1, 1];
        normal = normals(i, :);
        if norm(normal - one) > norm(normal + one)
            is_minus = 1;
        else
            is_minus = -1;
        end
        if is_minus == 1
            w = one - normal;
        else
            w = one + normal;
        end
        w = w / norm(w);
        Q = eye(3) - 2 * wedge(w);
        M = (Q') * shape_operators(i) * Q;
        mean_curvature(i) = M(2, 2) + M(3, 3);
        if ~isnan(mean_curvature(i))
            max_mean_curvature = max(max_mean_curvature, abs(mean_curvature(i)));
            min_mean_curvature = min(min_mean_curvature, abs(mean_curvature(i)));
        end
    end
    %disp(mean_curvature);

    for i = 1 : vertex_size
        [R, G, B] = mean_curvature_to_rgb(mean_curvature(i), min_mean_curvature, max_mean_curvature);
        if isreal(R) && ~isnan(R)
            fprintf('%d, %d\n', i, R);
            if R >= 1.0
                R = 1.0
            end
            plot3(vertices(i, 1), vertices(i, 2), vertices(i, 3), '.', 'color', [R, G, B]);
            hold on, axis equal, axis off, axis image, hidden on;
        end
    end

end

function W = wedge(w)
    W = [0 -w(3) w(2);
          w(3) 0 -w(1);
         -w(2) w(1) 0];
end

function [R, G, B] = mean_curvature_to_rgb(mean_curvature, min_mean_curvature, max_mean_curvature)
    Y = 255.0 * ((log(1e-8 + abs(mean_curvature)) - log(1e-8 + min_mean_curvature))) / (log(1e-8 + max_mean_curvature) - log(1e-8 + min_mean_curvature));
%{
    R = Y + 1.4075 * 127;
    G = Y - 0.3455 * 127 - 0.7169 * 127;
    B = Y + 1.7790 * 127;

    if mean_curvature < 0
        R = 255 - R;
        B = B * 0.8;
    end

    R = (R - 127) / 255;
    G = (G - 127) / 255;
    B = (B - 127) / 255;

    R = (R + 1) / 2;
    G = (G + 1) / 2;
    B = (B + 1) / 2;
%}
    R = Y / 255;
    G = 0;
    B = 0;
end
















% end
