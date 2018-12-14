
window.CINDER_WATCHER = {

    callbacks:{},

    /**
     * Adds 
     * @param {string} id an id identifying the file you want to watch. Can be a full path, will get quashed down to filename 
     * @param {*} fn the callback function to run when the file is changed. 
     */
    addFile:function( id,fn )
    {
        // first construct an id if necessary 
        id = id.split("/");

        id = id[id.length - 1];  
        
        this.callbacks[id] = fn;
    },

    /**
     * Processes the server response. 
     * @param {string} id the id to look up so we know which callback function to run. 
     */
    processServerResponse:function( id )
    {

        let cb = this.callbacks[id];
        cb(id);
    }

};