function main()
    [vertex, faces] = obj__read('test.obj');
    plot_mesh(vertex, faces);
    shading interp;
    for i=1:length(vertex)
        [D, S, Q] = perform_fast_marching_mesh(vertex, faces, i);
    end
end
    