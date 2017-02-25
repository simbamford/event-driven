#include <iCub/eventdriven/vWindow.h>

namespace ev {

vSurface2::vSurface2(int width, int height)
{
    this->width = width;
    this->height = height;
    this->count = 0;

    spatial.resize(height);
    for(int y = 0; y < height; y++) {
        spatial[y].resize(width);
    }
}

void vSurface2::fastAddEvent(event <> v, bool onlyAdd)
{
    event<AddressEvent> c = std::static_pointer_cast<AddressEvent>(v);
    if(c->y >= height || c->x >= width) {
        return;
    }

    if(!onlyAdd)
        fastRemoveEvents(v);

    q.push_back(v);

    if(!spatial[c->y][c->x])
        count++;

    spatial[c->y][c->x] = c;

    return;


}

vQueue vSurface2::addEvent(event<> v)
{
    event<AddressEvent> c = std::static_pointer_cast<AddressEvent>(v);
    if(c->y >= height || c->x >= width) {
        //std::cout << "WHY" << std::endl;
        return vQueue();
    }

    vQueue removed = removeEvents(v);

    q.push_back(v);
    if(c) {

        if(spatial[c->y][c->x])
            removed.push_back(spatial[c->y][c->x]);
        else
            count++;

        spatial[c->y][c->x] = c;
    }

    return removed;

}

vQueue vSurface2::getSurf()
{
    return getSurf(0, width, 0, height);
}

vQueue vSurface2::getSurf(int d)
{
    event<AddressEvent> v(nullptr);
    for(vQueue::reverse_iterator qi = q.rbegin(); qi != q.rend(); qi++) {
        v = getas<AddressEvent>(*qi);
        if(v) break;
    }
    if(!v) return vQueue();

    return getSurf(v->x, v->y, d);

}

vQueue vSurface2::getSurf(int x, int y, int d)
{
    return getSurf(x - d, x + d, y - d, y + d);
}

vQueue vSurface2::getSurf(int xl, int xh, int yl, int yh)
{
    vQueue qcopy;

    xl = std::max(xl, 0);
    xh = std::min(xh, width-1);
    yl = std::max(yl, 0);
    yh = std::min(yh, height-1);

    for(int y = yl; y <= yh; y++)
        for(int x = xl; x <= xh; x++)
            if(spatial[y][x]) qcopy.push_back(spatial[y][x]);

    return qcopy;

}

void vSurface2::getSurfSorted(vQueue &fillq)
{
    fillq.resize(count);
    if(!count) return;

    unsigned int i = 0;
    vQueue::reverse_iterator rqit;
    for(rqit = q.rbegin(); rqit != q.rend(); rqit++) {
        event<AddressEvent> v = std::static_pointer_cast<AddressEvent>(*rqit);
        if(v != spatial[v->y][v->x]) continue;
        fillq[i++] = v;
    }
}

vQueue vSurface2::getSurf_Tlim(int dt)
{
    return getSurf_Tlim(dt, 0, width, 0, height);
}

vQueue vSurface2::getSurf_Tlim(int dt, int d)
{
    event<AddressEvent> v(nullptr);
    for(vQueue::reverse_iterator qi = q.rbegin(); qi != q.rend(); qi++) {
        v = getas<AddressEvent>(*qi);
        if(v) break;
    }
    if(!v) return vQueue();

    return getSurf_Tlim(dt, v->x, v->y, d);

}

vQueue vSurface2::getSurf_Tlim(int dt, int x, int y, int d)
{
    return getSurf_Tlim(dt, x - d, x + d, y - d, y + d);

}

vQueue vSurface2::getSurf_Tlim(int dt, int xl, int xh, int yl, int yh)
{
    vQueue qcopy;
    if(q.empty()) return qcopy;

    int t = q.back()->stamp;

    for(vQueue::reverse_iterator rqit = q.rbegin(); rqit != q.rend(); rqit++) {

        //check it is on the surface
        event<AddressEvent> v = std::static_pointer_cast<AddressEvent>(*rqit);
        if(v != spatial[v->y][v->x]) continue;

        //check temporal constraint
        int vt = (*rqit)->stamp;
        if(vt > t) vt -= vtsHelper::max_stamp;
        if(vt + dt <= t) break;

        //check spatial constraint
        if(v->x >= xl && v->x <= xh) {
            if(v->y >= yl && v->y <= yh) {
                qcopy.push_back(v);
            }
        }
    }

    return qcopy;
}

//vQueue vSurface2::getSurf_Tlim(int dt, int xl, int xh, int yl, int yh)
//{
//    vQueue qcopy;
//    if(q.empty()) return qcopy;
//    int t = q.back()->getStamp();
//    int vt = 0;

//    xl = std::max(xl, 0);
//    xh = std::min(xh, width-1);
//    yl = std::max(yl, 0);
//    yh = std::min(yh, height-1);

//    for(int y = yl; y <= yh; y++) {
//        for(int x = xl; x <= xh; x++) {
//            if(spatial[y][x]) {
//                vt = spatial[y][x]->getStamp();
//                if(vt > t) vt -= vtsHelper::maxStamp();
//                if(vt + dt > t)
//                    qcopy.push_back(spatial[y][x]);
//            }
//        }
//    }

//    return qcopy;
//}

vQueue vSurface2::getSurf_Clim(int c)
{
    return getSurf_Clim(c, 0, width, 0, height);
}

vQueue vSurface2::getSurf_Clim(int c, int d)
{
    event<AddressEvent> v(nullptr);
    for(vQueue::reverse_iterator qi = q.rbegin(); qi != q.rend(); qi++) {
        v = getas<AddressEvent>(*qi);
        if(v) break;
    }
    if(!v) return vQueue();

    return getSurf_Clim(c, v->getX(), v->getY(), d);

}

vQueue vSurface2::getSurf_Clim(int c, int x, int y, int d)
{
    return getSurf_Clim(c, x - d, x + d, y - d, y + d);

}

vQueue vSurface2::getSurf_Clim(int c, int xl, int xh, int yl, int yh)
{
    vQueue qcopy;
    if(q.empty()) return qcopy;

    xl = std::max(xl, 0);
    xh = std::min(xh, width-1);
    yl = std::max(yl, 0);
    yh = std::min(yh, height-1);

    for(int y = yl; y <= yh; y++) {
        for(int x = xl; x <= xh; x++) {
            if(spatial[y][x]) qcopy.push_back(spatial[y][x]);
        }
    }

    qcopy.sort(true);
    while(qcopy.size() > (unsigned int)c)
        qcopy.pop_front();

    return qcopy;
}


event<> vSurface2::getMostRecent()
{
    if(!q.size()) return NULL;
    return q.back();
}

/******************************************************************************/
vQueue temporalSurface::removeEvents(event<> toAdd)
{
    vQueue removed;

    //calculate event window boundaries based on latest timestamp
    int ctime = toAdd->stamp;
    int upper = ctime + vtsHelper::max_stamp - duration;
    int lower = ctime - duration;

    //remove any events falling out the back of the window
    while(q.size()) {

        event<AddressEvent> v = std::static_pointer_cast<AddressEvent>(q.front());
        if(v && v != spatial[v->y][v->x]) {
            q.pop_front();
            continue;
        }

        int vtime = q.front()->stamp;

        if((vtime > ctime && vtime < upper) || vtime < lower) {
            removed.push_back(q.front());
            if(v) spatial[v->y][v->x] = NULL;
            q.pop_front();
            count--;
        } else {
            break;
        }
    }

    while(q.size()) {

        event<AddressEvent> v = std::static_pointer_cast<AddressEvent>(q.back());
        if(v && v != spatial[v->y][v->x]) {
            q.pop_back();
            continue;
        }

        int vtime = q.back()->stamp;

        if((vtime > ctime && vtime < upper) || vtime < lower) {
            removed.push_back(q.back());
            if(v) spatial[v->y][v->x] = NULL;
            q.pop_back();
            count--;
        } else {
            break;
        }
    }

    return removed;
}

/******************************************************************************/
void temporalSurface::fastRemoveEvents(event<> toAdd)
{

    //calculate event window boundaries based on latest timestamp
    int ctime = toAdd->stamp;
    int upper = ctime + vtsHelper::max_stamp - duration;
    int lower = ctime - duration;

    //remove any events falling out the back of the window
    while(q.size()) {

        event<AddressEvent> v = std::static_pointer_cast<AddressEvent>(q.front());
        if(v && v != spatial[v->y][v->x]) {
            q.pop_front();
            continue;
        }

        int vtime = v->stamp;

        if((vtime > ctime && vtime < upper) || vtime < lower) {
            if(v) spatial[v->y][v->x] = NULL;
            q.pop_front();
            count--;
        } else {
            break;
        }
    }

//    while(q.size()) {

//        event<AddressEvent> v = std::static_pointer_cast<AddressEvent>(q.back());
//        if(v && v != spatial[v->y][v->x]) {
//            q.pop_back();
//            continue;
//        }

//        int vtime = v->stamp;

//        if((vtime > ctime && vtime < upper) || vtime < lower) {
//            if(v) spatial[v->y][v->x] = NULL;
//            q.pop_back();
//            count--;
//        } else {
//            break;
//        }
//    }
}

/******************************************************************************/
vQueue fixedSurface::removeEvents(event<> toAdd)
{

    vQueue removed;

    while(q.size()) {

        event<AddressEvent> v = getas<AddressEvent>(q.front());
        if(v && v != spatial[v->getY()][v->getX()]) {
            q.pop_front();
        } else {
            break;
        }
    }

    if(count > qlength) {

        removed.push_back(q.front());
        event<AddressEvent> v = getas<AddressEvent>(q.front());
        if(v) spatial[v->getY()][v->getX()] = NULL;
        q.pop_front();
        count--;
    }

    return removed;

}

void fixedSurface::fastRemoveEvents(event<> toAdd)
{

    while(q.size()) {

        event<AddressEvent> v = getas<AddressEvent>(q.front());
        if(v && v != spatial[v->getY()][v->getX()]) {
            q.pop_front();
        } else {
            break;
        }
    }

    if(count > qlength) {

        event<AddressEvent> v = getas<AddressEvent>(q.front());
        if(v) spatial[v->getY()][v->getX()] = NULL;
        q.pop_front();
        count--;
    }

}

/******************************************************************************/
vQueue lifetimeSurface::addEvent(event<> toAdd)
{

    event<FlowEvent> v = getas<FlowEvent>(toAdd);
    if(!v) return vQueue();
    return vSurface2::addEvent(v);
}

vQueue lifetimeSurface::removeEvents(event<> toAdd)
{

    vQueue removed;
    //this could be better if q is sorted by death.


    //lifetime requires a flow event only
    event<FlowEvent> toAddflow = getas<FlowEvent>(toAdd);
    if(!toAddflow)
        return vQueue();

    int cts = toAddflow->getStamp();
    int cx = toAddflow->getX(); int cy = toAddflow->getY();


    vQueue::iterator i = q.begin();
    while(i != q.end()) {
        event<FlowEvent> v = std::static_pointer_cast<FlowEvent>(*i);
        int modts = cts;
        if(cts < v->getStamp()) //we have wrapped
            modts += vtsHelper::maxStamp();

        bool samelocation = v->getX() == cx && v->getY() == cy;

        if(modts > v->getDeath() || samelocation) {
            //it could be dangerous if spatial gets more than 1 event per pixel
            removed.push_back(*i);
            spatial[v->getY()][v->getX()] = NULL;
            i = q.erase(i);
            count--;
        } else {
            i++;
        }
    }

    return removed;

}

void lifetimeSurface::fastRemoveEvents(event<> toAdd)
{

    //lifetime requires a flow event only
    event<FlowEvent> toAddflow = getas<FlowEvent>(toAdd);
    if(!toAddflow)
        return;

    int cts = toAddflow->getStamp();
    int cx = toAddflow->getX(); int cy = toAddflow->getY();


    vQueue::iterator i = q.begin();
    while(i != q.end()) {
        event<FlowEvent> v = std::static_pointer_cast<FlowEvent>(*i);
        int modts = cts;
        if(cts < v->getStamp()) //we have wrapped
            modts += vtsHelper::maxStamp();

        bool samelocation = v->getX() == cx && v->getY() == cy;

        if(modts > v->getDeath() || samelocation) {
            //it could be dangerous if spatial gets more than 1 event per pixel
            spatial[v->getY()][v->getX()] = NULL;
            i = q.erase(i);
            count--;
        } else {
            i++;
        }
    }

}


}
