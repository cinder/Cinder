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
        .pipe( gulp.dest( './../html' ) );
});

gulp.task( 'watch', [ 'styles' ], function() {

    gulp.watch( 'stylus/*.styl', [ 'styles' ] );
});

gulp.task( 'default', [ 'watch' ] );

gulp.task( 'build', [
	'styles'
	] );