#include <gst/gst.h>

int meain(int argc, char *argv[]) 
{
	GstElement *pipeline;
	GstMessage *msg;

	gst_init(&argc, &argv);

	GstElement *videoSrc;
	GstElement *videoBalance;
	GstElement *x264enc;
	GstElement *capsFilter;
	GstElement *rtph264pay;
	GstElement *udpSink;

	pipeline = gst_pipeline_new("stream_to_android");
	videoSrc = gst_element_factory_make("videotestsrc", "videoSrc");
	videoBalance = gst_element_factory_make("videobalance", "videoBalance");
	g_object_set(G_OBJECT(videoBalance), "saturation", 0.5, NULL);
	x264enc = gst_element_factory_make("x264enc", "x264enc");
	g_object_set(G_OBJECT(x264enc), "aud", FALSE, NULL);
	g_object_set(G_OBJECT(x264enc), "tune", 4, NULL);
	g_object_set(G_OBJECT(x264enc), "speed-preset", 1, NULL);
	g_object_set(G_OBJECT(x264enc), "pass", 5, NULL);
	g_object_set(G_OBJECT(x264enc), "bitrate", 8192, NULL);
	g_object_set(G_OBJECT(x264enc), "quantizer", 22, NULL);
	g_object_set(G_OBJECT(x264enc), "byte-stream", TRUE, NULL);
	g_object_set(G_OBJECT(x264enc), "threads", 2, NULL);
	capsFilter = gst_element_factory_make("capsfilter", "capsFilter");
	GstCaps *caps = gst_caps_new_simple("video/x-h264",
		"width", G_TYPE_INT,640,
		"height", G_TYPE_INT, 480,
		"stream-format", G_TYPE_STRING, "byte-stream",
		"framerate", GST_TYPE_FRACTION, 10, 1,
		NULL);
	g_object_set(G_OBJECT(capsFilter), "caps", caps, NULL);
	rtph264pay = gst_element_factory_make("rtph264pay", "rtph264pay");
	g_object_set(G_OBJECT(rtph264pay), "pt", 96, NULL);
	udpSink = gst_element_factory_make("udpsink", "udpSink");
	g_object_set(G_OBJECT(udpSink), "host", "10.22.221.156", NULL); /*10.22.221.156*/ /*172.17.129.117*/
	g_object_set(G_OBJECT(udpSink), "port", 1238, NULL);
	g_object_set(G_OBJECT(udpSink), "buffer-size", 4096, NULL);
	g_object_set(G_OBJECT(udpSink), "sync", FALSE, NULL);
	g_object_set(G_OBJECT(udpSink), "async", FALSE, NULL);

	gst_bin_add_many(GST_BIN(pipeline), videoSrc, videoBalance, x264enc, capsFilter, rtph264pay, udpSink, NULL);
	gboolean b = gst_element_link_many(videoSrc, videoBalance, x264enc, capsFilter, rtph264pay, udpSink, NULL);
	g_print("All pipeline elements linked: %d", b);

	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	msg = gst_bus_timed_pop_filtered(GST_ELEMENT_BUS(pipeline), GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

	if (msg != NULL) {
		g_print("Pipeline bus receiver message %p", msg);
		switch(GST_MESSAGE_TYPE(msg)) {
			case GST_MESSAGE_ERROR:
				gchar *debugInfo = NULL;
				GError *err = NULL;
				gst_message_parse_error(msg, &err, &debugInfo);
				g_print("Error message received from %s: %s", GST_MESSAGE_SRC(msg->src), err->message);
				g_print("Debug Information: %s", debugInfo ? debugInfo : "None");
				g_clear_error(err);
				g_free(debugInfo);
				break;
			case GST_MESSAGE_EOS:
				g_print("Message EOS");
				break;
			default:
				g_print("Unkonw Message");
				break;
		}
		gst_object_unref(msg);
	}

	gst_element_set_state(pipeline, GST_STATE_NULL);
	
	gst_object_unref(videoSrc);
	gst_object_unref(videoBalance);
	gst_object_unref(x264enc);
	gst_object_unref(capsFilter);
	gst_object_unref(caps);
	gst_object_unref(rtph264pay);
	gst_object_unref(udpSink);
	gst_object_unref(pipeline);

	return 0;
}