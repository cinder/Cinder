var section;
$(document).ready(function() {

	var cinderJs = this;

	// set the current section in the nav
	// console.log( document.location.href.split("html/")[1] );
	// if()

	
	// look for all dom items with class

 	// find all expandable sections
 	var expandableSections = $( '.expandable' );
 	expandableSections.each( function() {

 		var $this = $(this);
 		var content = $( $this.find('.content')[0] );
 		var maxHeight = content.innerHeight();

 		// set height to max height
 		content.css('height', maxHeight + 'px');

 		// add a button
 		var button = $('<button class="expand"></button>');
 		$this.prepend( button );

 		button.on('click', function(){
 			$this.toggleClass('hidden');
 		}); 		
 		
 		// Hide all of the definition tags
 		if( $this.find( ".functionDef, .enumDef, .definitionCol" ).length > 0 ){
 			$this.addClass("hidden");
 		}
 	});

 	var setSection = function( sectionName ){
 		// find the nav item that correlates to the section name
 		// console.log( "CUR SECTION", sectionName );
 		// console.log( $("#main-nav").find("li#nav_"+sectionName) );
 		$("#main-nav").find("li#nav_"+sectionName).addClass( "current" );
 	};

 	var showAll = function(){
 		$( '.contents .expandable' ).each( function(){
 			var $this = $(this);
 			$this.removeClass("hidden");
 		});
 	};

 	var hideAll = function(){
 		$( '.contents .expandable' ).each( function(){
 			var $this = $(this);
 			$this.addClass("hidden");
 		});
 	};

 	// attach show/hide all functionality
 	$( '#show-hide a.show-all' ).on( 'click', showAll );
 	$( '#show-hide a.hide-all' ).on( 'click', hideAll );
 	console.log($( '#show-hide a.show-all' ));
 	

 	setSection( section );
 	return cinderJs;

 } );

// Calling foundation runs through any calls
// related to foundation 5.
// One such thing is making sure the column sizes stay in sync
try{
	$(document).foundation();	
} catch( e ){

}

window.setSection = function( sectionName ){	
	section = sectionName;
}
