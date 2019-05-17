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

//-------------------------------------------------------------------
void ofxSequencer::setup(int cols, int beatsPerMinute, int beatsPerBar)
{
    ofSetLogLevel(OF_LOG_NOTICE);
    
    this->cols = cols;// total num cols
    setBpm(beatsPerMinute, beatsPerBar);
    setMouseActive(true);
    setPosition(0, 0, 24 * cols, 96);
    
    //--

    // TODO: maybe should change from vector to array fixed size mode..
    // maybe to use pointers too

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
void ofxSequencer::get_gridFromSequencerParams()
{
//    // put grid values (bool) from internal parameters (bool)
//
//    ofLogNotice("ofxSequencer")<< "-------------get_gridFromSequencerParams-----------";
//
//    //--
//
//    for (int r = 0; r < rows.size(); r++)
//    {
//        ofLogVerbose("ofxSequencer") << "--- row " << r;
//
//        // read and fill all bools cols for any rows?
//        for (int c = 0 ; c < cols; c++)
//        {
//            bool boolState;
//            boolState = rows[r]->grid_Rows[c];
//
//            grid[c][r] = boolState;
//
//            ofLogVerbose("ofxSequencer") << "grid c:" << c << " = " << boolState;
//        }
//    }
//
//    //--
}

//-------------------------------------------------------------------
void ofxSequencer::DEBUG_All_GRID()
{
    // dump grid matrix values

    ofLogNotice("ofxSequencer")<< "DEBUG_All_GRID";
    
    bool myVal;
    for (int r = 0; r < rows.size(); r++)
    {
        ofLogVerbose("ofxSequencer") << "--- row " << r;
          
        for (int c = 0; c < cols; c++)
        {
            myVal = grid[r][c];
              
            ofLogVerbose("ofxSequencer") << "- col " << c << " " << myVal;
        }
    }
    ofLogVerbose("ofxSequencer") << "-------------------------------------------" ;
}

//-------------------------------------------------------------------
void ofxSequencer::set_GridFromSequencer()
{
    // get values from internal parameters to data grid vector ?

    ofLogNotice("ofxSequencer") << "set_GridFromSequencer";
    
    for (int r = 0; r < rows.size(); r++) {
        ofLogVerbose("ofxSequencer") << "--- row " << r;
        
        // save every row columns values on his object..
        rows[r]->store_Row_Values();//working
        
        //-
        
        for (int c = 0; c < cols; c++)
        {
            //ofLogVerbose("ofxSequencer") << "r:" << r << " c:" << c;
            
            bool myVal;
            myVal = rows[r]->grid_Rows[c];//test after move to base
         
            grid[r][c] = myVal;
        }
        ofLogVerbose("ofxSequencer") << "----------------------------";
    }
}

//------------------------------------------------
void ofxSequencer::set_SequencerFromGrid()
{
    ofLogNotice("ofxSequencer") << "set_SequencerFromGrid";

    // put sequencer parameters values (bool) from grid vector (bool) !

    for (int n = 0; n < rows.size(); n++)
    {
        string str;

        ofLogNotice("ofxSequencer") << "row:" << n;
        for (int b = 0; b < cols; b++)
        {
            // SEQUENCER CLASS

            // set value to sequencer grid squares

            bool state = grid[n][b];

            setValue<bool>( n, b, state );

            str += ofToString( state ) + " ";

            //-
        }

        ofLogNotice("ofxSequencer") << str;
    }
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

//-------------------------------------------------------------------
void ofxSequencer::stepBack()
{
    column = (column - 1);// % cols;
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

// MOUSE HANDLERS:

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
        
        ofLogVerbose("ofxSequencer") << "< pressed: mCell x " << mCell.x;
        ofLogVerbose("ofxSequencer") << "< pressed: mCell y " << mCell.y;
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
        
        ofLogVerbose("ofxSequencer") << "< released: mCell x " << mCell.x;
        ofLogVerbose("ofxSequencer") << "< released: mCell y " << mCell.y;
    }
    else {
        draggingFrames = 0;
    }
    draggingCell = false;
    toRedraw = true;
}

//-------------------------------------------------------------------
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

//-------------------------------------------------------------------
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

//-------------------------------------------------------------------
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
    //ofSetLineWidth(4);
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

//-------------------------------------------------------------------
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


