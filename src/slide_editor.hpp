#pragma once
#include <QWidget>
#include "slide.hpp"

class QLineEdit;
class QComboBox;
class QListWidget;
class QPushButton;
class QDoubleSpinBox;
class QGroupBox;

class SlideEditor : public QWidget {
    Q_OBJECT
public:
    explicit SlideEditor(QWidget* parent = nullptr);
    void setSlide(const Slide& s);
    Slide slide() const { return m_slide; }

    void selectImage(int idx);
    void refreshFromDeckImage(int idx, const SlideImage& img);

signals:
    void slideChanged();
    void imageSelected(int idx);

private slots:
    void onBgColorChanged();
    void onImageAdd();
    void onImageRemove();
    void onImageSelected(int row);
    void onImageGeometryChanged();
    void onCropChanged();

private:
    void refreshImageList();
    void updateImageEditorState();

    Slide m_slide;
    QLineEdit* m_bgColorEdit;
    QListWidget* m_imageList;
    QPushButton* m_addImageBtn;
    QPushButton* m_removeImageBtn;
    QDoubleSpinBox* m_imgX, *m_imgY, *m_imgW, *m_imgH;
    QDoubleSpinBox* m_cropX, *m_cropY, *m_cropW, *m_cropH;
    QGroupBox* m_imgControls;
    QGroupBox* m_cropControls;
    int m_selectedImage = -1;
    bool m_updating = false;
};