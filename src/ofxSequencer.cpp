#include "ofxSequencer.h"


ofxSequencerRowBase::ofxSequencerRowBase(int cols)
{
    this->cols = cols;
}

ofxSequencer::ofxSequencer()
{
    toRedraw = true;
}

ofxSequencer::~ofxSequencer()
{
    stop();
}

void ofxSequencer::setup(int cols, int beatsPerMinute, int beatsPerBar)
{
    this->cols = cols;
    setBpm(beatsPerMinute, beatsPerBar);
    setMouseActive(true);
    setPosition(0, 0, 24 * cols, 96);
    
    //--
    
//    // resize and erase bools grid. 12 rows and 16 cols
//    for (int r = 0; r < 12; r++)
//    {
//        vector <bool> myBools;
//        for (int c = 0 ; c < 16; c++)
//        {
//            myBools.push_back(false);
//        }
//        GRID_values.push_back(myBools);//create row
//    }
    
    //--
}

void ofxSequencer::setBpm(int beatsPerMinute, int beatsPerBar)
{
    this->beatsPerMinute = beatsPerMinute;
    bpm.setBpm(beatsPerMinute);
    bpm.setBeatPerBar(beatsPerBar);
    bpmInterval = 60000.0 / beatsPerMinute;
}

void ofxSequencer::start()
{
    ofAddListener(bpm.beatEvent, this, &ofxSequencer::play);
    bpm.start();
}

void ofxSequencer::stop()
{
    ofRemoveListener(bpm.beatEvent, this, &ofxSequencer::play);
    bpm.stop();
}

void ofxSequencer::reset()
{
    bpm.reset();
    column = 0;
}

void ofxSequencer::randomize()
{
    for (int r=0; r<rows.size(); r++) {
        rows[r]->randomize();
    }
    toRedraw = true;
}

//-------------------------------------------------------------------

void ofxSequencer::DEBUG_All_GRID()
{
    cout << "-------------DEBUG_All_GRID-----------" << endl;
    
    bool myVal;
    for (int r = 0; r < rows.size(); r++)
    {
        cout << "--- row " << r << endl;
          
        for (int c = 0; c < cols; c++)
        {
            myVal = GRID_RowsByCols_values[r][c];
              
            cout << "- col " << c << " " << myVal << endl;
        }
    }
    cout << "-------------------------------------------" << endl;
}

void ofxSequencer::get_AllValues()
{
//    rows[r]->store_allGridValues();
    cout << "-------------get_AllValues-----------" << endl;
    
    for (int r = 0; r < rows.size(); r++) {
        cout << "--- row " << r << endl;
        
        // save every row columns values on his object..
        rows[r]->store_Row_Values();//working
        
        //-
        
        for (int c = 0; c < cols; c++)
        {
            //cout << "r:" << r << " c:" << c << endl;
            
            bool myVal;
            
            myVal = rows[r]->GRID_row_Values[c];//test after move to base
            
//            ((ofxSequencerRow<T>*) rows[r])->getValue_Cell(c);
//            rows[r]->getValue_Cell<ofxSequencerRow>(c);
            
//            myVal = rows[r]->getValue_Cell(c);//not working
//            myVal = rows[r]->get_CellValue(c);//not working
            
//            myVal = get;
            
//            myVal = (<ofxSequencerRow> rows[r])->get_Value(c);//error
//            (this->rows[r])->getValue<ofxSequencerRow>())
         
             GRID_RowsByCols_values[r][c] = myVal;
            
        }

        cout << "----------------------------" << endl;
    }
//    toRedraw = true;
}

//-------------------------------------------------------------------

void ofxSequencer::play(void)
{
    advance();
}

void ofxSequencer::advance()
{
    column = (column + 1) % cols;
    if (smooth) {
        bpmTime = ofGetElapsedTimeMillis();
        cursor = column;
        for (auto r : rows) {
            r->update(cursor);
        }
    }
    else
    {
        for (auto r : rows) {
            r->update(column);
        }
    }

    ofNotifyEvent(beatEvent, column, this);
}

void ofxSequencer::stepBack()
{
    column = (column - 1) % cols;
    if (column < 0) column = 0;
    
    if (smooth) {
        bpmTime = ofGetElapsedTimeMillis();
        cursor = column;
        for (auto r : rows) {
            r->update(cursor);
        }
    }
    else
    {
        for (auto r : rows) {
            r->update(column);
        }
    }
    
    ofNotifyEvent(beatEvent, column, this);
}

//-------------------------------------------------------------------

void ofxSequencer::setMouseActive(bool active)
{
    if (active)
    {
        ofAddListener(ofEvents().mousePressed, this, &ofxSequencer::mousePressed);
        ofAddListener(ofEvents().mouseReleased, this, &ofxSequencer::mouseReleased);
        ofAddListener(ofEvents().mouseDragged, this, &ofxSequencer::mouseDragged);
    }
    else
    {
        ofRemoveListener(ofEvents().mousePressed, this, &ofxSequencer::mousePressed);
        ofRemoveListener(ofEvents().mouseReleased, this, &ofxSequencer::mouseReleased);
        ofRemoveListener(ofEvents().mouseDragged, this, &ofxSequencer::mouseDragged);
    }
}

void ofxSequencer::mousePressed(ofMouseEventArgs &evt)
{
    ofRectangle seqRect(x, y, width, height);
    if (seqRect.inside(evt.x, evt.y))
    {
        mCell.set(floor((evt.x - x) / cellWidth),
                  floor((evt.y - y) / cellHeight));
        rows[mCell.y]->mousePressed(mCell.x, evt.x, evt.y);
        draggingCell = true;
        
        cout << ">pressed: mCell x " << mCell.x << endl;
        cout << ">pressed: mCell y " << mCell.y << endl;
        
    }
}

void ofxSequencer::mouseDragged(ofMouseEventArgs &evt)
{
    if (draggingCell)
    {
        rows[mCell.y]->mouseDragged(mCell.x, ofGetMouseY());
        draggingFrames++;
        toRedraw = true;
    }
}

void ofxSequencer::mouseReleased(ofMouseEventArgs &evt)
{
    if (draggingCell && draggingFrames==0) {
        rows[mCell.y]->mouseReleased(mCell.x);
    }
    else {
        draggingFrames = 0;
    }
    draggingCell = false;
    toRedraw = true;
    
    cout << "<released: mCell x " << mCell.x << endl;
    cout << "<released: mCell y " << mCell.y << endl;
}

void ofxSequencer::setPosition(int x, int y, int width, int height)
{
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    fbo.allocate(width, height);
    fbo.begin();
    ofClear(0, 0);
    fbo.end();
    toRedraw = true;
}

void ofxSequencer::update()
{
    if (smooth && bpm.isPlaying())
    {
        cursor = column + (float) (ofGetElapsedTimeMillis() - bpmTime) / bpmInterval;
        for (auto r : rows) {
            r->update(cursor);
        }
    }
}

void ofxSequencer::draw()
{
    if (toRedraw) {
        redraw();
        toRedraw = false;
    }
    
    ofPushMatrix();
    ofPushStyle();
    
    ofTranslate(x, y);
    ofSetColor(255);
    fbo.draw(0, 0, width, height);
    
    ofSetRectMode(OF_RECTMODE_CORNER);
//    ofSetLineWidth(4);
    ofSetLineWidth(2);
    ofNoFill();

    if (bpm.isPlaying() && smooth)
    {
        float t = cursor - floor(cursor);
        ofSetColor(255, 0, 0, 255 * (1 - t));
        ofDrawRectangle(cellWidth * column, 0, cellWidth, height);
        ofSetColor(255, 0, 0, 255 * t);
        ofDrawRectangle(cellWidth * ((column + 1) % cols), 0, cellWidth, height);
    }
    else
    {
//        ofSetColor(255, 0, 0);
//        // lines only
//        ofNoFill();
//        ofSetColor(ofColor::white);
//        ofDrawRectangle(cellWidth * column, 0, cellWidth, height);
        
        // filled bar with alpha
        ofFill();
        int grey = 255;
        int a = 64;
        ofColor c;
        c.set(grey);
        ofSetColor(c.r, c.g, c.b, a);
        ofDrawRectangle(cellWidth * column, 0, cellWidth, height);
        
    }
    
    ofPopStyle();
    ofPopMatrix();
    
//    //debug
//    ofSetColor(ofColor::white);
//    ofDrawBitmapString(ofToString(column), 800, 800);
////    if (column == 4) column = 0;
}

void ofxSequencer::redraw()
{
    cellWidth  = (float) width  / cols;
    cellHeight = (float) height / rows.size();

    fbo.begin();
    
    ofPushMatrix();
    ofPushStyle();
    
    ofSetColor(0);
    ofFill();
    ofDrawRectangle(0, 0, width + 120, height);
    ofSetColor(255);

    ofSetRectMode(OF_RECTMODE_CENTER);
    ofTranslate(0.5 * cellWidth, 0.5 * cellHeight);
    for (int r=0; r<rows.size(); r++)
    {
        ofSetColor(255);
        for (int c = 0; c < cols; c++)
        {
            rows[r]->draw(c, cellWidth, cellHeight);
            ofTranslate(cellWidth, 0);
        }
        ofTranslate(-cols * cellWidth, cellHeight);
//        ofSetColor(0, 200, 0);
        ofSetColor(ofColor::white);
        ofDrawBitmapString(rows[r]->getName(), -20 -0.5 * cellWidth + 2, -cellHeight + 8);
    }
    ofTranslate(-0.5*cellWidth, (-0.5-rows.size())*cellHeight);
    
    ofSetColor(100);
    ofSetLineWidth(1);
    for (int r=1; r<rows.size(); r++) {
        ofDrawLine(0, r * cellHeight, width, r * cellHeight);
    }
    for (int c=1; c<cols; c++) {
        ofDrawLine(c * cellWidth, 0, c * cellWidth, height);
    }
    
    ofPopStyle();
    ofPopMatrix();
    
    fbo.end();
}

//void ofxSequencer::getGRID()
//{
////    rows[mCell.y]->mousePressed(mCell.x, evt.x, evt.y);
////    draggingCell = true;
//    
//    for (int r=0; r<rows.size(); r++)
//    {
////        ofLogVerbose() << "> ROW " << ofToString(r);
////        
//        for (int c = 0; c < cols; c++)
//        {
//   
//            ofLogVerbose() << " > COL " << ofToString(c);
//            
//
//
//            
////            auto *myRow = rows[r]->getMax<<#class T#>>();
//
//            
//            
//            
////            ofLogVerbose() << " > getName[r]" << ofToString(rows[r]->getName());
//
////            
//////            rows[r]->getValue(c);
//
////            
//        }
////
//   
//    }
//}

