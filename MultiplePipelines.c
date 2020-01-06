#include <gst/gst.h>
#include <glib.h>

static gpointer dump_1(gpointer data)
{
	for (int i = 0; i < 10; i++) {
		g_print("dump_1: %d\n", i);
	}
	return NULL;
}

static gpointer dump_2(gpointer data) {
	for (int i = 0; i < 10; i++) {
		g_print("dump_2: %d\n", i);
	}
	return NULL;
}


static gpointer run_pipeline_1(gpointer data)
{
	g_print("run_pipeline_1\n");
	GstElement* pipeline;
	GstBus *bus;
	GstMessage *msg;

	pipeline = gst_parse_launch("videotestsrc pattern=ball ! autovideosink", NULL);

	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	bus = gst_element_get_bus(pipeline);
	msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

	if (msg != NULL)
	{
		g_print("Pipeline_1 bus receiver message %p", msg);
		switch (GST_MESSAGE_TYPE(msg)) {
		case GST_MESSAGE_ERROR:
			gchar *debugInfo = NULL;
			GError *err = NULL;
			gst_message_parse_error(msg, &err, &debugInfo);
			g_print("Error message received from %s: %s", GST_MESSAGE_SRC(msg->src), err->message);
			g_print("\n");
			g_print("Debug Information: %s", debugInfo ? debugInfo : "None");
			g_clear_error(&err);
			//if (debugInfo != NULL)
			//	g_free(debugInfo);
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
	gst_object_unref(bus);
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);
	return NULL;
}

static gpointer run_pipeline_2(gpointer data)
{
	g_print("run_pipeline_2\n");
	GstElement* pipeline;
	GstBus *bus;
	GstMessage *msg;

	pipeline = gst_parse_launch("videotestsrc ! autovideosink", NULL);

	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	bus = gst_element_get_bus(pipeline);
	msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

	if (msg != NULL)
	{
		g_print("Pipeline_2 bus receiver message %p", msg);
		switch (GST_MESSAGE_TYPE(msg)) {
		case GST_MESSAGE_ERROR:
			gchar *debugInfo = NULL;
			GError *err = NULL;
			gst_message_parse_error(msg, &err, &debugInfo);
			g_print("Error message received from %s: %s", GST_MESSAGE_SRC(msg->src), err->message);
			g_print("\n");
			g_print("Debug Information: %s", debugInfo ? debugInfo : "None");
			g_clear_error(&err);
			//if (debugInfo != NULL)
			//	g_free(debugInfo);
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
	gst_object_unref(bus);
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);
	return NULL;
}


static gpointer main_loop_1(gpointer data)
{
	g_print("main_loop_1\n");
	GMainContext *context;
	GstElement *pipeline;

	context = g_main_context_new();

	pipeline = gst_parse_launch("videotestsrc pattern=ball ! autovideosink", NULL);
	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	g_print("Entering main_loop_1\n");
	GMainLoop *mainLoop = g_main_loop_new(context, FALSE);
	g_main_loop_run(mainLoop);
	g_print("Exiting main_loop_1\n");
	return NULL;

}

static gpointer main_loop_2(gpointer data)
{
	g_print("main_loop_2\n");
	GMainContext *context;
	GstElement *pipeline;

	context = g_main_context_new();

	pipeline = gst_parse_launch("videotestsrc ! autovideosink", NULL);
	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	g_print("Entering main_loop_2\n");
	GMainLoop *mainLoop = g_main_loop_new(context, FALSE);
	g_main_loop_run(mainLoop);
	g_print("Exiting main_loop_2\n");
	return NULL;
}

static gpointer pipe_updsink(gpointer data)
{
	g_print("pipe_updsink\n");
	GMainContext *context;
	GstElement *pipeline;

	context = g_main_context_new();

	//pipeline = gst_parse_launch("filesrc location=C:/Users/halimeh/Desktop/ProceduralDemo.mp4 ! decodebin name=dec ! autovideosink", NULL);
	//pipeline = gst_parse_launch("videotestsrc pattern=ball ! x264enc ! video/x-h264, width=640, height=480, stream-format=byte-stream, framerate=30/1 ! rtph264pay pt=96 ! udpsink host=172.17.129.117 port=1240", NULL);
	//pipeline = gst_parse_launch("filesrc location=C:/Users/halimeh/Desktop/ProceduralDemo.mp4 ! decodebin name=dec ! x264enc aud=false speed-preset=1 tune=4 pass=5 quantizer=22 byte-stream=true threads=4 bitrate=4096 ! video/x-h264, stream-format=byte-stream ! rtph264pay pt=96 ! udpsink host=172.17.129.117 port=1240 buffer-size=4096", NULL);
	//pipeline = gst_parse_launch("filesrc location=C:/Users/halimeh/Desktop/ProceduralDemo.mp4 ! decodebin name=dec ! queue2 ! x264enc aud=false speed-preset=1 tune=4 pass=5 quantizer=22 byte-stream=true threads=4 bitrate=4096 ! video/x-h264, stream-format=byte-stream ! rtph264pay pt=96 ! udpsink host=172.17.129.117 port=1240", NULL);
	//pipeline = gst_parse_launch("filesrc location=C:/Users/halimeh/Desktop/ProceduralDemo.mp4 ! decodebin name=dec ! tee name=t t. ! queue2 ! autovideosink t. ! queue2 ! x264enc aud=false speed-preset=1 tune=4 pass=5 quantizer=22 byte-stream=true threads=4 bitrate=4048 ! video/x-h264, stream-format=byte-stream ! rtph264pay pt=96 ! queue2 ! udpsink host=172.17.129.117 port=1240", NULL);
	//pipeline = gst_parse_launch("filesrc location=C:/Users/halimeh/Desktop/ProceduralDemo.mp4 ! decodebin name=dec ! tee name=t t. ! queue2 ! autovideosink t. ! queue2 ! x264enc aud=false speed-preset=1 tune=4 pass=5 quantizer=22 byte-stream=true threads=2 bitrate=8048 ! video/x-h264, stream-format=byte-stream ! queue2 ! rtph264pay pt=96 ! queue2 ! udpsink host=172.17.129.117 port=1240 sync=true", NULL);
	pipeline = gst_parse_launch("filesrc location=C:/Users/halimeh/Desktop/ProceduralDemo.mp4 ! decodebin name=dec ! tee name=t t. ! queue2 ! timeoverlay time-mode=0 text=Sender ! autovideosink t. ! queue2 ! timecodestamper ! videoconvert ! videorate ! video/x-raw, framerate=25/1 ! x264enc aud=false speed-preset=1 tune=4 pass=5 quantizer=22 byte-stream=true threads=2 bitrate=4096 ! video/x-h264, stream-format=byte-stream ! rtph264pay pt=96 ! udpsink host=172.17.129.117 port=1240 sync=false buffer-size=4096", NULL);
	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	g_print("Entering main_loop_1\n");
	GMainLoop *mainLoop = g_main_loop_new(context, FALSE);
	g_main_loop_run(mainLoop);
	g_print("Exiting main_loop_1\n");
	g_main_loop_unref(mainLoop);
	g_main_context_unref(context);
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);
	return NULL;
}

static gpointer pipe_udpsrc(gpointer data)
{
	g_print("pipe_udpsrc\n");
	GMainContext *context;
	GstElement *pipeline;

	context = g_main_context_new();

	//pipeline = gst_parse_launch("udpsrc port=1241 ! application/x-rtp, encoding-name=H264, payload=96 ! rtpjitterbuffer latency=50 mode=1 ! queue2 ! rtph264depay ! h264parse ! avdec_h264 max-threads=4 skip-frame=1 ! videoconvert ! autovideosink sync=false async=false", NULL);
	//pipeline = gst_parse_launch("udpsrc port=1241 ! application/x-rtp, encoding-name=H264, payload=96 ! rtpjitterbuffer latency=50 mode=1 ! queue2 ! rtph264depay ! h264parse ! avdec_h264 max-threads=4 skip-frame=1 ! autovideosink sync=false async=false", NULL);
	//pipeline = gst_parse_launch("udpsrc port=1241 ! application/x-rtp, encoding-name=H264, payload=96 ! rtpjitterbuffer mode=4 ! queue2 ! rtph264depay ! h264parse ! avdec_h264 max-threads=4 ! autovideosink sync=false async=false", NULL);
	pipeline = gst_parse_launch("udpsrc port=1241 ! application/x-rtp, encoding-name=H264, payload=96 ! rtpjitterbuffer latency=50 mode=1 ! rtph264depay ! h264parse disable-passthrouhg=true ! queue ! avdec_h264 skip-frame=1 lowres=2 ! timeoverlay time-mode=0 text=Receiver ! autovideosink sync=false async=false", NULL);
	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	g_print("Entering main_loop_2\n");
	GMainLoop *mainLoop = g_main_loop_new(context, FALSE);
	g_main_loop_run(mainLoop);
	g_print("Exiting main_loop_2\n");
	g_main_loop_unref(mainLoop);
	g_main_context_unref(context);
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);

	return NULL;
}

static gpointer pipe_udpsrc_updsink(gpointer data)
{
	g_print("pipe_udpsrc_udpsink\n");
	GMainContext *context;
	GstElement  *pipeline;

	context = g_main_context_new();

	pipeline = gst_parse_launch("udpsrc port=1240 ! application/x-rtp, encoding-name=H264, payload=96 ! rtpjitterbuffer latency=50 mode=1 ! rtph264depay ! h264parse disable-passthrough=true ! avdec_h264 skip-frame=1 max-threads=2 ! tee name=t t. ! queue ! videobalance saturation=0.25 ! videoconvert ! autovideosink sync=false t. ! queue ! videobalance saturation=0.0 ! x264enc aud=false speed-preset=1 tune=4 pass=5 quantizer=22 byte-stram=true threads=1 bitrate=4096 ! video/x-h264, stream-format=byte-stream ! rtph264pay pt=96 ! udpsink host=172.17.129.117 port=1241 buffer-size=4096 sync=false", NULL);
	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	g_print("Entering main_loop_3\n");
	GMainLoop *mainLoop = g_main_loop_new(context, FALSE);
	g_main_loop_run(mainLoop);
	g_print("Exiting main_loop_3\n");
	g_main_loop_unref(mainLoop);
	g_main_context_unref(context);
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);

	return NULL;
}

int main(int argc, char *argv[])
{
	gst_init(&argc, &argv);

	GError *err;
	GThread *t1 = g_thread_try_new("t1", pipe_updsink, NULL, &err);
	GThread *t2 = g_thread_try_new("t2", pipe_udpsrc, NULL, &err);
	GThread *t3 = g_thread_try_new("t3", pipe_udpsrc_updsink, NULL, &err);

	g_thread_join(t1);
	g_thread_join(t2);
	g_thread_join(t3);
	
	//g_assert_no_error(err);
	if (err != NULL) {
		//g_clear_error(&err);
	}
}