function [] = random_viewpoint_generator()

    obj = read_wobj('C:\Users\AndrewHuang\Documents\GitHub\notes-on-algorithms\view selection\EXP_MODELS_LR\desk_chair_LR.obj');

    FV.vertice = obj.vertices;

    FV.faces = obj.objects(length(obj.objects)).data.vertices;

    figure, patch(FV, 'facecolor', [0.5, 0.5, 0.5]), axis equal, axis off, axis image, hidden on;

    cnt = 1;
    for x = -1:0.4:1
        for y = -1:0.4:1
            for z = -1:0.4:1
                view([x, y, z]);
                saveas(gcf, ['C:\Users\AndrewHuang\Documents\GitHub\notes-on-algorithms\view selection\res\' num2str(cnt) '.png']);
                cnt = cnt + 1;
            end
        end
    end

    %[rn, ~] = get_points();

    %for ii = 1 : length(rn)
    %    light('color', [0.5, 0.5, 0.5], 'style', 'local', 'position',rn(ii, :));
    %    view(rn(ii, :));
    %    saveas(gcf, ['C:\Users\AndrewHuang\Documents\GitHub\notes-on-algorithms\view selection\res\' num2str(ii,'%04d') '.png']);

    %end
end

function [rn, vn] = get_points()
    N = 1024;
    a = rand(N, 1) * 2 * pi;
    b = asin(rand(N, 1) * 2 - 1);
    r0 = [cos(a).*cos(b), sin(a).*cos(b), sin(b)];
    v0 = zeros(size(r0));
    G = 1e-2;

    for ii = 1 : 256
         [rn, vn] = countnext(r0, v0, G);
         r0 = rn;
         v0 = vn;
    end

 end


function [rn, vn] = countnext(r, v, G)
    num = size(r, 1);
    dd = zeros(3, num, num);

    for m = 1 : num - 1
        for n = m + 1 : num
            dd(:, m, n) = (r(m, :) - r(n, :))';
            dd(:, n, m) = -dd(:, m, n);
        end
    end

    L = sqrt(sum(dd.^2, 1));
    L(L<1e-2) = 1e-2;
    F = sum(dd./repmat(L.^3, [3 1 1]), 3)';

    Fr = r.*repmat(dot(F, r, 2), [1 3]);
    Fv = F - Fr;

    rn=r + v;
    rn=rn./repmat(sqrt(sum(rn.^2, 2)), [1 3]);
    vn = v + G * Fv;
end
