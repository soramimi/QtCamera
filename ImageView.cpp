
#include "ImageView.h"
#include <QAbstractVideoSurface>
#include <QCamera>
#include <QPainter>
#include <QVideoSurfaceFormat>

class MyVideoSurface : public QAbstractVideoSurface {
public:
	ImageView *view = nullptr;
	QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType type) const
	{
		if (type == QAbstractVideoBuffer::NoHandle) {
			return {
				QVideoFrame::Format_ARGB32,
				QVideoFrame::Format_ARGB32_Premultiplied,
				QVideoFrame::Format_RGB32,
				QVideoFrame::Format_RGB24,
				QVideoFrame::Format_RGB565,
				QVideoFrame::Format_RGB555,
				QVideoFrame::Format_ARGB8565_Premultiplied,
				QVideoFrame::Format_BGRA32,
				QVideoFrame::Format_BGRA32_Premultiplied,
				QVideoFrame::Format_BGR32,
				QVideoFrame::Format_BGR24,
				QVideoFrame::Format_BGR565,
				QVideoFrame::Format_BGR555,
				QVideoFrame::Format_AYUV444,
				QVideoFrame::Format_AYUV444_Premultiplied,
				QVideoFrame::Format_YUV444,
				QVideoFrame::Format_YUV420P,
				QVideoFrame::Format_YV12,
				QVideoFrame::Format_UYVY,
				QVideoFrame::Format_YUYV,
				QVideoFrame::Format_NV12,
				QVideoFrame::Format_NV21,
				QVideoFrame::Format_IMC1,
				QVideoFrame::Format_IMC2,
				QVideoFrame::Format_Y8,
				QVideoFrame::Format_Y16,
				QVideoFrame::Format_Jpeg,
				QVideoFrame::Format_CameraRaw,
				QVideoFrame::Format_AdobeDng,
			};
		} else {
			return QList<QVideoFrame::PixelFormat>();
		}
	}
	bool present(const QVideoFrame &frame)
	{
		if (view) {
			return view->present(frame);
		}
		return false;
	}
};


struct ImageView::Private {
	QCamera camera;
	QVideoSurfaceFormat format;
	MyVideoSurface surface;
	QImage image;
};

ImageView::ImageView(QWidget *parent)
	: QWidget(parent)
	, m(new Private)
{
	m->surface.view = this;
	m->format = QVideoSurfaceFormat({640, 480}, QVideoFrame::Format_RGB24);
	m->format = m->surface.nearestFormat(m->format);
	m->surface.start(m->format);
	m->camera.setViewfinder(&m->surface);
	m->camera.start();
}

ImageView::~ImageView()
{
	m->camera.stop();
	delete m;
}

void ImageView::paintEvent(QPaintEvent *event)
{
	QPainter pr(this);
	pr.drawImage(0, 0, m->image);
}

bool ImageView::present(const QVideoFrame &frame)
{
	if (frame.isValid()) {
		if (frame.pixelFormat() == QVideoFrame::Format_BGR24) {
			m->image = frame.image();
			int w = frame.width();
			int h = frame.height();
			int linesize = m->image.bytesPerLine();
			for (int y = 0; y < h / 2; y++) {
				uint8_t *a = m->image.scanLine(y);
				uint8_t *b = m->image.scanLine(h - 1 - y);
				for (int x = 0; x < linesize; x++) {
					std::swap(a[x], b[x]);
				}
			}
			update();
		}
		return true;
	}
	return true;
}

