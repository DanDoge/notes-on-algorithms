obj = read_wobj( );

FV.vertice = obj.vertices

FV.faces = obj.objects(3).data.vertices;

figure, patch(FV, 'facecolor', [ , , ]); canlight; view([ , ]);
