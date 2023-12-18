#include <gst/gst.h>
#include <stdio.h>

int t_main(int argc, char *argv[]){
    GstElement *pipeline, *source, *sink, *filter, *videoconvert;
    GstBus *bus;
    GstMessage *msg;
    GstStateChangeReturn ret;

    gst_init(&argc,&argv);

    source = gst_element_factory_make("videotestsrc","source");
    filter = gst_element_factory_make("vertigotv","filter");
    videoconvert = gst_element_factory_make("videoconvert","videoconvert");
    sink = gst_element_factory_make("autovideosink","sink");

    pipeline = gst_pipeline_new("test-pipeline");

    if (!pipeline || !source || !sink || !filter || !videoconvert){
        g_printerr("Not all elements could be created\n");
        return -1;
    }

    gst_bin_add_many(GST_BIN(pipeline),source,filter,videoconvert,sink,NULL);

    if (gst_element_link(source,filter)!=TRUE || 
        gst_element_link(filter,videoconvert)!=TRUE ||
        gst_element_link(videoconvert,sink)!=TRUE
    ) {
        g_printerr("Elements could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }
    gint pt;
    printf("Enter pattern type: ");
    scanf("%d",&pt);
    g_object_set(source,"pattern",pt,NULL);

    gfloat speed;
    printf("Enter speed of video (default: 0.02): ");
    scanf("%f",&speed);
    g_object_set(filter,"zoom-speed",speed,NULL);

    ret = gst_element_set_state(pipeline,GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE){
        g_printerr("Unable to set state playing");
        gst_object_unref(pipeline);
        return -1;
    }

    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(
        bus,
        GST_CLOCK_TIME_NONE,
        GST_MESSAGE_ERROR | GST_MESSAGE_EOS
    );
    
    if (msg!=NULL){
        GError *err;
        gchar *debug_info;
        switch (GST_MESSAGE_TYPE(msg)){
            case GST_MESSAGE_ERROR:
                gst_message_parse_error(msg,&err,&debug_info);
                g_printerr(
                    "Error received from element %s: %s\n",
                    GST_OBJECT_NAME(msg->src),
                    err->message
                );
                g_printerr("Debugging information: %s\n",debug_info?debug_info:"none");
                g_clear_error(&err);
                g_free(debug_info);
                break;
            case GST_MESSAGE_EOS:
                g_print("End of stream reached.\n");
                break;
            default:
                g_printerr("Unexpected message received.\n");
                break;
        }
        gst_message_unref(msg);
    }
    
    gst_object_unref(bus);
    gst_element_set_state(pipeline,GST_STATE_NULL);
    gst_object_unref(pipeline);
    return 0;
}
int main(int argc, char *argv[]){
    return t_main(argc,argv);
}