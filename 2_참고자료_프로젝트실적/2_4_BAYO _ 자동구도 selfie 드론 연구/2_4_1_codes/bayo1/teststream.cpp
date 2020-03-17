#include < windows.h>
#include < vlc/vlc.h>

using namespace cv;
using namespace std;

struct ctx
{
	Mat* image;
	HANDLE mutex;
	uchar* pixels;
};
bool isRunning = true;

Size getsize(const char* path)
{
	libvlc_instance_t *vlcInstance;
	libvlc_media_player_t *mp;
	libvlc_media_t *media;

	char *vlc_argv[6];
	vlc_argv[0] = new char[3];
	strcpy_s(vlc_argv[0], 3, "-I");
	vlc_argv[1] = new char[6];
	strcpy_s(vlc_argv[1], 6, "dummy"); // Don't use any interface
	vlc_argv[2] = new char[16];
	strcpy_s(vlc_argv[2], 16, "--ignore-config"); // Don't use VLC's config
	vlc_argv[3] = new char[128];
	strcpy_s(vlc_argv[3], 128, "--plugin-path=/plugins");
	int vlc_argc = 4;

	vlcInstance = libvlc_new(vlc_argc, vlc_argv);

	for (int i = 0; i < vlc_argc; i++)
		delete[] vlc_argv[i];

	media = libvlc_media_new_location(vlcInstance, path);
	mp = libvlc_media_player_new_from_media(media);

	libvlc_media_release(media);
	libvlc_video_set_callbacks(mp, NULL, NULL, NULL, NULL);
	libvlc_video_set_format(mp, "RV24", 100, 100, 100 * 24 / 8); // pitch = width * BitsPerPixel / 8
																 //libvlc_video_set_format(mp, "RV32", 100, 100, 100 * 4);
	libvlc_media_player_play(mp);

	Sleep(2000);//wait a while so that something get rendered so that size info is available
	unsigned int width = 640, height = 480;

	for (int i = 0; i < 30 && height == 0; i++)
	{
		Sleep(50);
		libvlc_video_get_size(mp, 0, &width, &height);

		if (width != 0 && height != 0)
			break;
	}


	if (width == 0 || height == 0)
	{
		width = 640;
		height = 480;
	}

	libvlc_media_player_stop(mp);
	libvlc_release(vlcInstance);
	libvlc_media_player_release(mp);
	return Size(width, height);
}


void *lock(void *data, void**p_pixels)
{
	struct ctx *ctx = (struct ctx*)data;
	WaitForSingleObject(ctx->mutex, INFINITE);
	*p_pixels = ctx->pixels;
	return NULL;

}

void display(void *data, void *id) {
	(void)data;
	assert(id == NULL);
}

void unlock(void *data, void *id, void *const *p_pixels)
{

	struct ctx *ctx = (struct ctx*)data;
	Mat frame = *ctx->image;
	if (frame.data)
	{
		imshow("frame", frame);
		if (waitKey(1) == 27)
		{
			isRunning = false;
			//exit(0);
		}
	}
	ReleaseMutex(ctx->mutex);
}


int main()
{
	string url = "rtsp://...";
	//vlc sdk does not know the video size until it is rendered, so need to play it a bit so that size is     known
	Size sz = getsize(url.c_str());

	// VLC pointers
	libvlc_instance_t *vlcInstance;
	libvlc_media_player_t *mp;
	libvlc_media_t *media;

	char *vlc_argv[6];
	vlc_argv[0] = new char[3];
	strcpy_s(vlc_argv[0], 3, "-I");
	vlc_argv[1] = new char[6];
	strcpy_s(vlc_argv[1], 6, "dummy"); // Don't use any interface
	vlc_argv[2] = new char[16];
	strcpy_s(vlc_argv[2], 16, "--ignore-config"); // Don't use VLC's config
	vlc_argv[3] = new char[128];
	strcpy_s(vlc_argv[3], 128, "--plugin-path=/plugins");
	int vlc_argc = 4;

	vlcInstance = libvlc_new(vlc_argc, vlc_argv);

	for (int i = 0; i < vlc_argc; i++)
		delete[] vlc_argv[i];

	media = libvlc_media_new_location(vlcInstance, url.c_str());
	mp = libvlc_media_player_new_from_media(media);
	libvlc_media_release(media);


	struct ctx* context = (struct ctx*)malloc(sizeof(*context));
	context->mutex = CreateMutex(NULL, FALSE, NULL);
	context->image = new Mat(sz.height, sz.width, CV_8UC3);
	context->pixels = (unsigned char *)context->image->data;

	libvlc_video_set_callbacks(mp, lock, unlock, display, context);
	libvlc_video_set_format(mp, "RV24", sz.width, sz.height, sz.width * 24 / 8); // pitch = width *     BitsPerPixel / 8
																				 //libvlc_video_set_format(mp, "RV32", sz.width, sz.height, sz.width * 4);

	libvlc_media_player_play(mp);
	while (isRunning)
	{
		//imshow("rtsp", *(context->image));
		Sleep(1);
	}

	libvlc_media_player_stop(mp);
	libvlc_release(vlcInstance);
	libvlc_media_player_release(mp);
	free(context);

	return 0;
}