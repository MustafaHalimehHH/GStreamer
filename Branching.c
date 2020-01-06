#include <gst/gst.h>

//GST_DEBUG_CATEGORY_STATIC(my_category);
//#define GST_CAT_DEFAULT my_category

int ddmain(int argc, char* argv[])
{
	GstElement *pipeline;
	//GstBus *bus;
	GstMessage *msg;

	gst_init(&argc, &argv);

	GstElement *tee;
	GstElement *queueUDP;
	GstElement *videoSrcUDP;
	GstElement *x264enc;
	GstElement *capsFilter;
	GstElement *rtph264pay;
	GstElement *udpSink;
	GstElement *queueLocal;
	GstElement *videoSrcLocal;
	GstElement *videoSink;

	GstPad *tee_udp_pad, *tee_local_pad;
	GstPad *queue_udp_pad, *queue_local_pad;

	pipeline = gst_pipeline_new("send_receive");

	tee = gst_element_factory_make("tee", "tee");
	queueUDP = gst_element_factory_make("queue", "queueUDP");
	videoSrcUDP = gst_element_factory_make("videotestsrc", "videoSrcUDP");
	x264enc = gst_element_factory_make("x264enc", "x264enc");
	g_object_set(G_OBJECT(x264enc), "aud", FALSE, NULL);
	g_object_set(G_OBJECT(x264enc), "tune", 4, NULL);
	g_object_set(G_OBJECT(x264enc), "speed-preset", 1, NULL);
	g_object_set(G_OBJECT(x264enc), "pass", 5, NULL);
	g_object_set(G_OBJECT(x264enc), "bitrate", 4096, NULL);
	g_object_set(G_OBJECT(x264enc), "quantizer", 22, NULL);
	g_object_set(G_OBJECT(x264enc), "byte-stream", TRUE, NULL);
	g_object_set(G_OBJECT(x264enc), "threads", 2, NULL);
	capsFilter = gst_element_factory_make("capsfilter", "capsFilter");
	GstCaps *caps = gst_caps_new_simple("video/x-h264",
		"width", G_TYPE_INT, 640,
		"height", G_TYPE_INT, 480,
		"stream-format", G_TYPE_STRING, "byte-stream",
		"framerate", GST_TYPE_FRACTION, 15, 1,
		NULL);
	g_object_set(G_OBJECT(capsFilter), "caps", caps, NULL);
	rtph264pay = gst_element_factory_make("rtph264pay", "rtph264pay");
	g_object_set(G_OBJECT(rtph264pay), "pt", 96, NULL);
	udpSink = gst_element_factory_make("udpsink", "udpSink");
	g_object_set(G_OBJECT(udpSink), "host", "172.17.129.117", NULL);
	g_object_set(G_OBJECT(udpSink), "port", 1240, NULL);
	queueLocal = gst_element_factory_make("queue", "queueLocal");
	videoSrcLocal = gst_element_factory_make("videotestsrc", "videoSrcLocal");
	videoSink = gst_element_factory_make("autovideosink", "videoSink");

	//gst_debug_set_threshold_for_name("GST_DEBUG", 6);
	//setenv("GST_DEBUG", "*:7", TRUE);
	//putenv("GST_DEBUG=5");
	//GST_DEBUG_CATEGORY_INIT(my_category, "my category", 0, "This is my very own");

	//pipeline = gst_parse_launch("vidreotestsrc ! autovideosink", NULL);
	//pipeline = gst_parse_launch("tee name=t .t ! queue ! videotestsrc ! autovideosink t. ! queue ! videotestsrc pattern=ball ! autovideosink", NULL);
	//pipeline = gst_parse_launch("tee name=t t. ! queue ! videotestsrc pattern=ball ! x264enc ! video/x-h264, width=640, height=480, stream-format=byte-stream, framerate=10/1 ! rtph264pay pt=96 ! udpsink host=172.17.129.117 port=1240 \
								t. ! queue ! udpsrc port=1241 ! application/x-rtp, encoding-name=H264, payload=96 ! rtpjitterbuffer ! rtph264depay ! h264parse ! avdec_h264 ! videoconvert ! autovideosink", NULL);
		//t. ! queue ! udpsrc port=1241 ! application/x-rtp, encoding-name=H264, payload=96 ! rtpjitterbuffer ! rtph264depay ! h264parse ! avdec_h264 ! videoconvert ! autovideosink", NULL);
		//t. ! queue ! videotestsrc ! autovideosink", NULL);
					

	if (!pipeline || !tee || !queueUDP || !videoSrcUDP || !x264enc || !capsFilter || !rtph264pay || !udpSink || !queueLocal || !videoSrcLocal || !videoSink) {
		g_printerr("Not All element could be created.\n");
			return -1;
	}

	gst_bin_add_many(GST_BIN(pipeline), tee, queueUDP, videoSrcUDP, x264enc, capsFilter, rtph264pay, udpSink, queueLocal, videoSrcLocal, videoSink, NULL);
	gboolean b = gst_element_link_many( queueUDP, x264enc, capsFilter, rtph264pay, udpSink, NULL);
	g_print("All pipeline elements linked : %d \n", b);

	gst_element_set_state(pipeline, GST_STATE_PLAYING);
	g_print("start\n");
	//bus = gst_element_get_bus(pipeline);
	msg = gst_bus_timed_pop_filtered(GST_ELEMENT_BUS(pipeline), GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);
	g_print("end\n");
	if (msg != NULL)
	{
		g_print("Pipeline bus receiver message %p\n", msg);
		switch (GST_MESSAGE_TYPE(msg)) {
		case GST_MESSAGE_ERROR:
			gchar *debugInfo;
			GError *err;
			gst_message_parse_error(msg, &err, &debugInfo);
			g_print("Error message received from %s: %s\n", GST_MESSAGE_SRC(msg->src), err->message);
			//g_print("\n");
			g_print("Debug Information: %s\n", debugInfo ? debugInfo : "None");
			g_clear_error(err);
			if (debugInfo != NULL)
				g_free(debugInfo);
			break;
		case GST_MESSAGE_EOS:
			g_print("Message EOS\n");
			break;
		default:
			g_print("Unkonw Message\n");
			break;
		}
		gst_object_unref(msg);
	}
	//gst_object_unref(bus);
	gst_element_set_state(pipeline, GST_STATE_NULL);
	//gst_object_unref(videoSrc);
	gst_object_unref(videoSink);
	gst_object_unref(pipeline);
	return 0;
}