// TITLE:   Optimization_Interface/path_graphics_item.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "path_graphics_item.h"

#include <QGraphicsScene>
#include <QGraphicsView>

namespace interface {

PathGraphicsItem::PathGraphicsItem(PathModelItem *model,
                                       QGraphicsItem *parent)
    : QGraphicsItem(parent) {
    // Set model
    this->model_ = model;
    this->initialize();
}

void PathGraphicsItem::initialize() {
    // Set pen
    this->pen_ = QPen(Qt::red);
    this->pen_.setWidth(2);

    // Set flags
    this->setFlags(QGraphicsItem::ItemSendsScenePositionChanges);
}

QRectF PathGraphicsItem::boundingRect() const {
    return this->shape().boundingRect();
}

void PathGraphicsItem::paint(QPainter *painter,
                               const QStyleOptionGraphicsItem *option,
                               QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // Draw current course
    painter->setPen(this->pen_);
    for (qint32 i = 1; i < this->model_->points_->length(); i++) {
        QLineF line(mapFromScene(*this->model_->points_->at(i-1)),
                    mapFromScene(*this->model_->points_->at(i)));
        painter->drawLine(line);
    }

    // Label with port
    if (!this->model_->points_->isEmpty() && this->model_->port_ != 0) {
        painter->setPen(Qt::black);
        QPointF text_pos(this->mapFromScene(*this->model_->points_->first()));
        painter->drawText(QRectF(text_pos.x(), text_pos.y(), 50, 15),
                          QString::number(this->model_->port_));
    }
}

void PathGraphicsItem::expandScene() {
    if (scene()) {
        // expand scene if item goes out of bounds
        QRectF newRect = this->sceneBoundingRect();
        QRectF rect = this->scene()->sceneRect();
        if (!rect.contains(newRect)) {
            this->scene()->setSceneRect(
                        this->scene()->sceneRect().united(newRect));
            if (!this->scene()->views().isEmpty()) {
                this->scene()->views().first()->setSceneRect(
                            this->scene()->sceneRect());
            }
        }
        this->scene()->update();
    }
}

QPainterPath PathGraphicsItem::shape() const {
    QPainterPath path;
    QPolygonF poly;
    for (QPointF *point : *this->model_->points_) {
        poly << *point;
    }
    path.addPolygon(poly);
    return path;
}

QVariant PathGraphicsItem::itemChange(GraphicsItemChange change,
                                        const QVariant &value) {
    if (change == ItemScenePositionHasChanged && scene()) {
        // check to expand the scene
        this->expandScene();
    }
    return QGraphicsItem::itemChange(change, value);
}

}  // namespace interface
