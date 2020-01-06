#include <gst/gst.h>

int qmain(int argc, char *argv[]) {
	GstElement *pipeline;
	GstMessage *msg;

	gst_init(&argc, &argv);

	GstElement *videoSrc;
	GstElement *videoBalance;
	GstElement *x264enc;
	GstElement *capsFilter;
	GstElement *rtph264pay;
	GstElement *ccpasFilter;
	GstElement *rtpjitterbuffer;
	GstElement *rtph264depay;
	GstElement *h264pay;
	GstElement *h264parse;
	GstElement *decodeBin;
	GstElement *avdec_h264;
	GstElement *mpegtsmux;
	GstElement *videoConvert;
	GstElement *videoSink;

	pipeline = gst_pipeline_new("encoding_decoding_h264");

	videoSrc = gst_element_factory_make("videotestsrc", "videoSrc");
	videoBalance = gst_element_factory_make("videobalance", "videoBalance");
	g_object_set(G_OBJECT(videoBalance), "saturation", 0.9, NULL);
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
		"width", G_TYPE_INT, 640,
		"height", G_TYPE_INT, 480,
		"stream-format", G_TYPE_STRING, "byte-stream",
		"framerate", GST_TYPE_FRACTION, 10, 1,
		NULL);
	g_object_set(G_OBJECT(capsFilter), "caps", caps, NULL);
	rtph264pay = gst_element_factory_make("rtph264pay", "rtph264pay");
	g_object_set(G_OBJECT(rtph264pay), "pt", 96, NULL);

	ccpasFilter = gst_element_factory_make("capsfilter", "ccapsfilter");
	GstCaps *ccaps = gst_caps_new_simple("application/x-rtp",
		"encoding-name", G_TYPE_STRING, "H264",
		"payload", G_TYPE_INT, 96,
		NULL);
	g_object_set(G_OBJECT(ccpasFilter), "caps", ccaps, NULL);
	rtpjitterbuffer = gst_element_factory_make("rtpjitterbuffer", "rtpjitterbuffer");
	rtph264depay = gst_element_factory_make("rtph264depay", "rtph264depay");
	h264parse = gst_element_factory_make("h264parse", "h264parse");
	decodeBin = gst_element_factory_make("decodebin", "decodeBin");
	avdec_h264 = gst_element_factory_make("avdec_h264", "avdec_h264");
	mpegtsmux = gst_element_factory_make("mpegtsmux", "mpegtsumx");
	videoConvert = gst_element_factory_make("videoconvert", "videoConvert");
	videoSink = gst_element_factory_make("glimagesink", "videoSink");


	gst_bin_add_many(GST_BIN(pipeline), videoSrc, videoBalance, x264enc, capsFilter, rtph264pay, ccpasFilter, rtph264depay, avdec_h264, videoSink, NULL);
	gboolean b = gst_element_link_many(videoSrc, videoBalance, x264enc, capsFilter, rtph264pay, ccpasFilter, rtph264depay, avdec_h264, videoSink, NULL);
		/*gst_bin_add_many(GST_BIN(pipeline), videoSrc, videoBalance, x264enc, capsFilter, h264parse, avdec_h264, videoConvert, videoSink, NULL);
	gboolean b = gst_element_link_many(videoSrc, videoBalance, x264enc, capsFilter, h264parse, avdec_h264, videoConvert, videoSink, NULL);*/

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
	gst_object_unref(x264enc);
	gst_object_unref(capsFilter);
	gst_object_unref(caps);
	gst_object_unref(rtph264pay);
	gst_object_unref(ccpasFilter);
	gst_object_unref(ccaps);
	gst_object_unref(rtpjitterbuffer);
	gst_object_unref(rtph264depay);
	gst_object_unref(h264parse);
	gst_object_unref(avdec_h264);
	gst_object_unref(videoConvert);
	gst_object_unref(videoSink);
	gst_object_unref(pipeline);

	return 0;
}