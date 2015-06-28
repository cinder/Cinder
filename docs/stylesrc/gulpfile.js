var gulp 			= require( 'gulp' );
var stylus 			= require( 'gulp-stylus' );
var nib 			= require( 'nib' );
var Filter 			= require( 'gulp-filter');


gulp.task( 'styles', function() {
    
    var filter = Filter('**/*.styl');

    return gulp.src( [ 
    	'./stylus/*.styl'
    	] )

    	.pipe(filter)
        .pipe( stylus({
            import: [ 'nib' ],
            use: [ nib() ]
        }))
        .pipe(filter.restore())
        .pipe( gulp.dest( './../html/_assets/css' ) )
        .pipe( gulp.dest( './../htmlsrc/_assets/css' ) );
});

gulp.task( 'scripts', function() {
    
    return gulp.src( [ 
        './../htmlsrc/_assets/js/*.js'
        ] )

        .pipe(gulp.dest('./../html/_assets/js'));
});

gulp.task( 'watch', [ 'styles' ], function() {
    gulp.watch( 'stylus/*.styl', [ 'styles' ] );
    gulp.watch( './../htmlsrc/_assets/js/*.js', [ 'scripts' ] );
});


gulp.task( 'default', [ 'watch' ] );

gulp.task( 'build', [
	'styles',
    'scripts'
	] );