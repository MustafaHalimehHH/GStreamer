#include <gst/gst.h>

int ffmain(int argc, char *argv[])
{
	GstElement *pipeline;
	GstMessage *msg;

	gst_init(&argc, &argv);

	GstElement *udpSrc;
	GstElement *capsFilter;
	GstElement *rtpjitterbuffer;
	GstElement *rtph264depay;
	GstElement *h264parse;
	GstElement *avdec_h264;
	GstElement *videoConvert;
	GstElement *videoSink;

	pipeline = gst_pipeline_new("receive_from_android");

	udpSrc = gst_element_factory_make("udpsrc", "udpSrc");
	g_object_set(G_OBJECT(udpSrc), "port", xxxx, NULL);
	capsFilter = gst_element_factory_make("capsfilter", "capsFilter");
	GstCaps *caps = gst_caps_new_simple("application/x-rtp",
		"encoding-name", G_TYPE_STRING, "H264",
		"payload", G_TYPE_INT, 96,
		NULL);
	g_object_set(G_OBJECT(capsFilter), "caps", caps, NULL);
	rtpjitterbuffer = gst_element_factory_make("rtpjitterbuffer", "rtpjitterbuffer");
	rtph264depay = gst_element_factory_make("rtph264depay", "rtph264depay");
	h264parse = gst_element_factory_make("h264parse", "h264parse");
	avdec_h264 = gst_element_factory_make("avdec_h264", "avdec_h264");
	videoConvert = gst_element_factory_make("videoconvert", "videoConvert");
	videoSink = gst_element_factory_make("autovideosink", "videoSink");

	gst_bin_add_many(GST_BIN(pipeline), udpSrc, capsFilter, rtpjitterbuffer, rtph264depay, h264parse, avdec_h264, videoConvert, videoSink, NULL);
	gboolean b = gst_element_link_many(udpSrc, capsFilter, rtpjitterbuffer, rtph264depay, h264parse, avdec_h264, videoConvert, videoSink, NULL);
	g_print("All pipeline elements linked: %d\n", b);

	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	msg = gst_bus_timed_pop_filtered(GST_ELEMENT_BUS(pipeline), GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

	if (msg != NULL) {
		g_print("Pipeline bus receiver message %p", msg);
		switch (GST_MESSAGE_TYPE(msg)) {
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
			g_print("Message EOS\n");
			break;
		default:
			g_print("Unkonw Message\n");
			break;
		}
		gst_object_unref(msg);
	}

	gst_element_set_state(pipeline, GST_STATE_NULL);

	gst_object_unref(udpSrc);
	gst_object_unref(caps);
	gst_object_unref(capsFilter);
	gst_object_unref(rtpjitterbuffer);
	gst_object_unref(rtph264depay);
	gst_object_unref(h264parse);
	gst_object_unref(avdec_h264);
	gst_object_unref(videoConvert);
	gst_object_unref(videoSink);
	gst_object_unref(pipeline);

	g_print("Done\n");

	return 0;
}