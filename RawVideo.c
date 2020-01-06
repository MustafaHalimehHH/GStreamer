#include <gst/gst.h>

int ymain(int argc, char *argv[]) {
	GstElement *pipeline;
	GstMessage *msg;

	gst_init(&argc, &argv);

	GstElement *videoSrc;
	GstElement *videoBalance;
	GstElement *capsFilter;
	GstElement *rtpvrawpay;
	GstElement *ccpasFilter;
	GstElement *rtpvrawdepay;
	GstElement *videoConvert;
	GstElement *videoSink;

	pipeline = gst_pipeline_new("encoding_decoding_raw");

	videoSrc = gst_element_factory_make("videotestsrc", "videoSrc");
	videoBalance = gst_element_factory_make("videobalance", "videoBalance");
	g_object_set(G_OBJECT(videoBalance), "saturation", 0.0, NULL);
	capsFilter = gst_element_factory_make("capsfilter", "capsFilter");
	GstCaps *caps = gst_caps_new_simple("video/x-raw",
		"width", G_TYPE_INT, 640,
		"height", G_TYPE_INT, 480,
		"stream-format", G_TYPE_STRING, "byte-stream",
		"framerate", GST_TYPE_FRACTION, 10, 1,
		NULL);
	g_object_set(G_OBJECT(capsFilter), "caps", caps, NULL);
	rtpvrawpay = gst_element_factory_make("rtpvrawpay", "rtpvrawpay");
	ccpasFilter = gst_element_factory_make("capsfilter", "ccapsfilter");
	GstCaps *ccaps = gst_caps_new_simple("application/x-rtp",
		"media", G_TYPE_STRING, "video",
		"clock-rate", G_TYPE_INT, 90000,
		"width", G_TYPE_INT, 640,
		"height", G_TYPE_INT, 480,
		"encoding-name", G_TYPE_STRING, "RAW",
		"payload", G_TYPE_INT, 96,
		NULL);
	g_object_set(G_OBJECT(ccpasFilter), "caps", ccaps, NULL);
	rtpvrawdepay = gst_element_factory_make("rtpvrawdepay", "rtpvrawdepay");
	videoConvert = gst_element_factory_make("videoconvert", "videoConvert");
	videoSink = gst_element_factory_make("glimagesink", "videoSink");


	gst_bin_add_many(GST_BIN(pipeline), videoSrc, videoBalance, capsFilter, rtpvrawpay, ccpasFilter, rtpvrawdepay, videoConvert, videoSink, NULL);
	gboolean b = gst_element_link_many(videoSrc, videoBalance, capsFilter, rtpvrawpay, ccpasFilter, rtpvrawdepay, videoConvert, videoSink, NULL);
	g_print("All pipeline elements linked: %d\n", b);

	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	msg = gst_bus_timed_pop_filtered(GST_ELEMENT_BUS(pipeline), GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

	if (msg != NULL) {
		g_print("Pipeline bus receiver message %p\n", msg);
		switch (GST_MESSAGE_TYPE(msg)) {
		case GST_MESSAGE_ERROR:
			g_print("Message Error\n");
			gchar *debugInfo = NULL;
			GError *err = NULL;
			gst_message_parse_error(msg, &err, &debugInfo);
			g_print("Error message received from %s: %s\n", GST_MESSAGE_SRC(msg->src), err->message);
			g_print("Debug Information: %s\n", debugInfo ? debugInfo : "None");
			g_clear_error(&err);
			g_free(debugInfo);
			break;
		case GST_MESSAGE_EOS:
			g_print("Message EOS\n");
			break;
		default:
			g_print("Unkonw Message Type\n");
			break;
		}
		gst_object_unref(msg);
	}


	gst_element_set_state(pipeline, GST_STATE_NULL);


	gst_object_unref(videoSrc);
	gst_object_unref(videoBalance);
	gst_object_unref(ccpasFilter);
	gst_object_unref(ccaps);
	gst_object_unref(videoConvert);
	gst_object_unref(videoSink);
	gst_object_unref(pipeline);

	return 0;
}