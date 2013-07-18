#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imagedisplay.h"
#include "imagelabel.h"

static const char *mat_types[] = {
    "char", "uchar", "short", "ushort", "int", "float", "double", "user"
};

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    //we have to register cv::Mat type so that
    qRegisterMetaType<cv::Mat>("cv::Mat");
    qRegisterMetaType<std::vector<cv::KeyPoint> >("std::vector<cv::KeyPoint>");

    //Qt can recognize it and is able to connect
    //Signals to Slots when using this type
    receiverThread = new ReceiverThread(this);

    connect(ui->imageDisplay,
            SIGNAL(Mouse_position(int, int)),
            this,
            SLOT(mouseMoved(int, int)));
    connect(receiverThread,
            SIGNAL(receivedImage(QString, cv::Mat)),
            this,
            SLOT(receivedImage(QString, cv::Mat)));
    connect(receiverThread,
            SIGNAL(receivedKeypoints(QString, std::vector<cv::KeyPoint>)),
            this,
            SLOT(receivedKeypoints(QString, std::vector<cv::KeyPoint>)));

    //connect(ui->imageDisplay,SIGNAL(zoomed()),this,SLOT(Zoomed_slot()));

    receiverThread->start(); //runs a new thread which launches server for
    //inter-process communication and listens to requests

    //variables inicialization
    representative_label_width = 150;
    representative_label_height = 90;
    label_description_height = 30;
    //maximum number of pictures stored in memory - ringbuffer
    max_item_count = 500;
    //variable counting number of received pictures
    item_counter = 0;

    QWidget *selectorWidget = new QWidget(this);

    //here we create a side panel of pictures
    //we put widgets into a layout so it makes no unexpected problems that while resizing
    selectorLayout = new QVBoxLayout(selectorWidget);
    selectorLayout->setAlignment(Qt::AlignTop);
    selectorLayout->setMargin(0);

    selectorWidget->setLayout(selectorLayout);
    ui->imageDisplay->setVisible(false);

    //we put our side panel to scrollarea to be easily able to scroll the pictures
    QScrollArea *left_panel_scrollarea = new QScrollArea(this);
    left_panel_scrollarea->setWidgetResizable(true);
    left_panel_scrollarea->setFixedWidth(representative_label_width + 18);
    left_panel_scrollarea->setBackgroundRole(QPalette::Light);
    left_panel_scrollarea->setWidget(selectorWidget);
    ui->selector->addWidget(left_panel_scrollarea);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    //setting size policy of whole window followed by appropriate picture repainting on the label
    ui->imageDisplay->setGeometry(0, 0, ui->groupBox->width(),
                                  ui->groupBox->height());
    if (received_items.size() > 0) { displayCurrentImage(); }
}

void MainWindow::closeEvent(QCloseEvent *)
{
    this->receiverThread->stop();
    this->receiverThread->wait();
}

void MainWindow::displayCurrentImage()
{
    ui->imageDisplay->setVisible(true);

    mutex.lock();

    cv::Mat &image = received_items[current_ID].displayableImage;

    QImage qImage = QImage((const unsigned char *)image.data,
                           image.cols, image.rows, image.step[0], QImage::Format_RGB888);

    //setting newly displayed picture's features to be displayed correctly
    ui->imageDisplay->setPixmap(QPixmap::fromImage(qImage).scaled(
                                    ui->groupBox->width(), ui->groupBox->height(),
                                    Qt::KeepAspectRatio)); //Qt::KeepAspectRatiobyexpanding
    ui->imageDisplay->setGeometry(0, 0, ui->imageDisplay->pixmap()->width(),
                                  ui->imageDisplay->pixmap()->height());

    QPixmap newPixmap = QPixmap::fromImage(qImage);
    if (newPixmap.size() == ui->imageDisplay->original_pixmap.size()) {
        // image of the same size, do not reset zoom settings
    } else {
        ui->imageDisplay->scalefactor_y = ui->imageDisplay->scaleFactor_x =
                                              (double) ui->imageDisplay->pixmap()->height() / (double) qImage.height();
        ui->imageDisplay->zoom_of_picture = 1;
        ui->imageDisplay->offset_x = 0;
        ui->imageDisplay->offset_y = 0;
    }
    ui->imageDisplay->original_pixmap = newPixmap;
    ui->imageDisplay->refreshImage();

    mutex.unlock();

    emit picture_changed(qImage, current_ID);
}

cv::Mat MainWindow::rescale(const cv::Mat &image)
{
    double min = 0, max = 1;
    cv::minMaxLoc(image, &min, &max);
    if (max == min) {
        max = min + 1;
    }
    cv::Mat res;
    cv::Mat tmp = ((image - min) / (max - min)) * 255;
    tmp.convertTo(res, CV_8UC1);
    return res;
}

//ked klikne na label, nacita sa do pointra image pixmapa a yobrayi sa na label
void MainWindow::reload_picture()
{
    ImageLabel *label = (ImageLabel *) sender();
    int newId = label->get_identity();
    if (newId != current_ID) {
        current_ID = newId;
        displayCurrentImage();
    }
}

void MainWindow::mouseMoved(int pos_x, int pos_y)
{
    QMutexLocker lock(&mutex);

    //counting accurate possition of mouse cursor on displayed image
    pos_x = (int)((pos_x / ui->imageDisplay->scaleFactor_x) +
                  ui->imageDisplay->offset_x);
    pos_y = (int)((pos_y / ui->imageDisplay->scalefactor_y) +
                  ui->imageDisplay->offset_y);

    cv::Mat &image = received_items[current_ID].displayableImage;

    QString values;

    //now we check every channel of displayed image at the position pointed by cursor
    // and add to variable "values" red, green and blue values of actual pixel
    //we have to recognize different types of images, therefore image.depth() is
    // assigned to variable type
    if (pos_x < image.cols && pos_y < image.rows) {
        int type = image.depth();
        int chans = image.channels();
        for (int ch = 0; ch < chans; ch++) {
            if (chans == 1) {
                values.append(" | Value=");
            } else {
                if (ch == 0) { values.append(" | R="); }
                else if (ch == 1) { values.append(" G="); }
                else if (ch == 2) { values.append(" B="); }
            }
            if (type == CV_8U)
                values.append(QString::number(image.at<unsigned char>(pos_y,
                                              pos_x * chans + ch)));
            else if (type == CV_8S) {
                values.append(QString::number(image.at<char>(pos_y, pos_x * chans + ch)));
            } else if (type == CV_16S) {
                values.append(QString::number(image.at<short>(pos_y, pos_x * chans + ch)));
            } else if (type == CV_16U)
                values.append(QString::number(image.at<unsigned short>(pos_y,
                                              pos_x * chans + ch)));
            else if (type == CV_32S) {
                values.append(QString::number(image.at<int>(pos_y, pos_x * chans + ch)));
            } else if (type == CV_32F)
                values.append(QString::number((double) image.at<float>(pos_y,
                                              pos_x * chans + ch)));
            else {
                values.append(QString::number(image.at<double>(pos_y, pos_x * chans + ch)));
            }
        }
    }

    //setting text of actual postition and RGB values on the lbl_position label
    ui->lbl_position->setText(QString("Pos: X = %1, Y = %2 %3").arg(pos_x).arg(
                                  pos_y).arg(values));
}

void MainWindow::receivedImage(QString name, cv::Mat image)
{
    mutex.lock();



    ReceivedItem &item = insertNewReceivedItem();
    item.type = ReceivedItem::TYPE_IMAGE;
    item.name = name;
    item.displayableImage = image;

    addLabelForCurrentItem();

    //in actual_image we store the image we are working with
    current_ID = item_counter;
    item_counter++;

    mutex.unlock();
    displayCurrentImage();
}

void MainWindow::receivedKeypoints(QString name,
                                   std::vector<cv::KeyPoint> points)
{
    mutex.lock();

    ReceivedItem &item = insertNewReceivedItem();
    item.type = ReceivedItem::TYPE_KEYPOINTS;
    item.name = name;

    cv::Mat originalImage;
    int foundId = findImageByName(name);
    if (foundId == -1) {
        int cols = 0, rows = 0;
        for (size_t i = 0; i < points.size(); i++) {
            const cv::KeyPoint &kp = points.at(i);
            if (kp.pt.x + kp.size > cols) {
                cols = kp.pt.x + kp.size;
            }
            if (kp.pt.y + kp.size > rows) {
                rows = kp.pt.y + kp.size;
            }
        }
        originalImage = cv::Mat(rows, cols, CV_8UC3, cv::Scalar::all(255));
    } else {
        originalImage = received_items[foundId].displayableImage;
    }

    cv::drawKeypoints(originalImage, points, item.displayableImage, cv::Scalar::all(-1),
                      cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

    addLabelForCurrentItem();

    //in actual_image we store the image we are working with
    current_ID = item_counter;
    item_counter++;

    mutex.unlock();
    displayCurrentImage();
}

/** Finds an image by its name. Starts at item_counter and traverses
 * backwards. Returns the index >= 0 if found, or -1 if not found.
 */
int MainWindow::findImageByName(QString name)
{
    if (received_items.size() == max_item_count) {
        int i = item_counter - 1;
        while (i != item_counter) {
            if (received_items[i].type == ReceivedItem::TYPE_IMAGE
                    && received_items[i].name == name) {
                return i;
            }
            i = (i - 1 + max_item_count) % max_item_count;
        }
    } else {
        for (int i = item_counter - 1; i >= 0; i++) {
            if (received_items[i].type == ReceivedItem::TYPE_IMAGE
                    && received_items[i].name == name) {
                return i;
            }
        }
    }
    return -1;
}


/** Make space for a new item.
 * Creates a new element in the list or deletes old item if more than `max_item_count` items in list.
 * After call, `received_items[item_counter]` represents the current item.
 */
ReceivedItem &MainWindow::insertNewReceivedItem()
{
    //if our list of items is full, we rewrite the oldest item in the list
    //- circular rewriting, we release representative labels out of the memory
    //just after we remove the labels from the left side panel
    if (received_items.size() > max_item_count) {
        item_counter = item_counter % max_item_count;
        received_items[item_counter] = ReceivedItem();

        selectorLayout->removeWidget(labels.at(0));
        selectorLayout->removeWidget(descriptions.at(0));

        // free the memory for label and description
        delete labels.takeFirst();
        delete descriptions.takeFirst();
    } else {
        //if our list of pictures is not full, we add new picture to the list
        // adds new picture to list of pictures in original format and quality
        received_items.push_back(ReceivedItem());
    }

    return received_items[item_counter];
}

void MainWindow::addLabelForCurrentItem()
{
    //in channels we store original number of channels before scaling
    cv::Mat &currentDisplayableImage = received_items[item_counter].displayableImage;
    int channels = currentDisplayableImage.channels();

    bool displayable = false;

    //in matrix_type we store type od data matrix is filled with -char,short, int, etc.
    int type = currentDisplayableImage.depth();
    const char *matrix_type = mat_types[type];


    if (type == CV_8S || type == CV_8U) {
        displayable = true;
    }

    //we scale the picture so that if it is not displayable type we scale it to displayable
    if (!displayable) {
        currentDisplayableImage = rescale(currentDisplayableImage);
    }

    //now we convert image format to suitable format for displaying our image
    //in actual_image we store the image we are working with
    if (channels == 3)
        cv::cvtColor(currentDisplayableImage,
                     currentDisplayableImage, cv::COLOR_BGR2RGB); //CV_BGR2RGB
    else
        cv::cvtColor(currentDisplayableImage,
                     currentDisplayableImage, cv::COLOR_GRAY2RGB); //CV_GRAY2RGB

    //converting image to suitable format for qt environment
    QImage qImage = QImage((const unsigned char *)
                           currentDisplayableImage.data,
                           currentDisplayableImage.cols,
                           currentDisplayableImage.rows,
                           currentDisplayableImage.step[0],
                           QImage::Format_RGB888);

    //creates new clickable label where the picture will be dislayed and sets its features
    ImageLabel *imageLabel = new ImageLabel(item_counter, this);
    imageLabel->setFixedSize(representative_label_width,
                             representative_label_height);
    imageLabel->setStyleSheet(ui->imageDisplay->styleSheet());
    imageLabel->setFrameShape(ui->imageDisplay->frameShape());
    imageLabel->setPixmap(QPixmap::fromImage(qImage).scaled(
                              representative_label_width, representative_label_width, Qt::KeepAspectRatio));

    //creates picture description and sets its features
    QLabel *description = new QLabel(this);
    description->setFixedSize(representative_label_width, label_description_height);

    QString desc = received_items[item_counter].name
                   + "\n("
                   + QString::number(currentDisplayableImage.cols)
                   + "x"
                   + QString::number(currentDisplayableImage.rows)
                   + "), "
                   + QString::number(channels) + ", " + matrix_type;
    description->setText(desc);

    //setting signal-slot mechanism for new created label
    //if clicked on a picture on the left panel, we load a picture to the main window
    //if a picture in a main window is changed, we have to set new features to windows where displayed
    connect(imageLabel, SIGNAL(clicked()), this, SLOT(reload_picture()));
    connect(this, SIGNAL(picture_changed(QImage, int)), imageLabel,
            SLOT(reload_picture(QImage, int)));

    //adding new labels to suitable list do that we can anytime get them when needed
    labels.push_back(imageLabel);
    descriptions.push_back(description);

    //adding new created label/picture with description on the top of the side panel
    selectorLayout->insertWidget(0, imageLabel);
    selectorLayout->insertWidget(0, description);
}
