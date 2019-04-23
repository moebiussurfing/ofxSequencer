

/// addon modified from the original from:
/// https://github.com/genekogan/ofxSequencer

#pragma once

#include "ofMain.h"
#include "ofxSequencerBpm.h"

//--

// T ofxSequencerRowBase

struct ofxSequencerRowBase
{
    ofxSequencerRowBase(int cols);
    
    virtual string getName()
    {
        string name; return name;
    }
    
    template<class T> T getMin();
    template<class T> T getMax();
    
    template<class T> T getValue();// my getter
    
    template<class T> void setValue(int idx, T value);

    bool getValue_Cell(int idx);// works?
    
    virtual void update(int column) { }
    virtual void update(float cursor) { }
    virtual void randomize() { }
    
    virtual void mousePressed(int col, int x, int y) { }
    virtual void mouseDragged(int col, int y) { }
    virtual void mouseReleased(int col) { }
    
    virtual void draw(int col, int cellWidth, int cellHeight) { }

    int cols;
    
    //-
    
    virtual void store_Row_Values() { }
    
    vector<bool> GRID_row_Values;
    
}; //ofxSequencerRowBase

//------------------

// T ofxSequencerRow

template<typename T>
struct ofxSequencerRow : public ofxSequencerRowBase
{
    ofxSequencerRow(ofParameter<T> * parameter, int cols);
    
    string getName()
    {
        return parameter->getName();
    }
    
    T getMin()
    {
        return parameter->getMin();
    }
    
    T getMax()
    {
        return parameter->getMax();
    }
    
    T getValue()
    {
        return parameter->getValue();
    }
    
    void setValue(int idx, T value)
    {
        values[idx] = value;
    }

    bool getValue_Cell(int idx)// works
    {
        bool myVal = (bool) values[idx];
        cout << "- getValue_Cell(idx) : " << myVal << endl;
        //return parameter->getMax();
        return myVal;//bool
    }
    
    //-
    
    bool get_CellValue(int idx)
    {
        bool myVal = (bool) values[idx];
        //cout << "get_CellValue[idx] : " << myVal << endl;
        return myVal;
    };
    
    void update(int column);
    void update(float cursor);
    
    void randomize();
    void store_Row_Values();
    
    void mousePressed(int col, int x, int y);
    void mouseDragged(int col, int y);
    void mouseReleased(int col);
    void draw(int col, int cellWidth, int cellHeight);
    
    //-
    
    ofParameter<T> * parameter;
    vector<T> values;
    T pValue;
    ofPoint pMouse;
    
    //-
    
}; //ofxSequencerRow

//------------------

// creator row with columns

template<class T>
ofxSequencerRow<T>::ofxSequencerRow(ofParameter<T> * parameter, int cols) : ofxSequencerRowBase(cols)
{
    this->parameter = parameter;
    for (int c=0; c<cols; c++)
    {
        values.push_back(parameter->get());
        
        //--
        
        // init row GRID_row_Values values vector
        GRID_row_Values.push_back( (bool) ( parameter->get() ) );
        
        //--
    }
} //ofxSequencerRow

//------------------

// update row parameter in current column

template<class T>
void ofxSequencerRow<T>::update(int column)
{
    *parameter = values[column];
}

//------------------

// update row parameter by cursor
template<class T>
void ofxSequencerRow<T>::update(float cursor)
{
    *parameter = ofLerp(values[(int) floor(cursor)], values[(int) ceil(cursor) % values.size()], cursor - floor(cursor));
}

//------------------

// randomize all columns (steps) in the row

template<class T>
void ofxSequencerRow<T>::randomize()
{
    for (int i = 0; i < values.size(); i++) {
        values[i] = ofRandom(parameter->getMin(), parameter->getMax());
        
        //--
        
        // TODO: add to GRID_row_Values too
        GRID_row_Values[i] = (bool) values[i];
        
        //-
    }
}

//------------------

// store all row columns values (steps) in bool vector GRID_row_Values

template<class T>
void ofxSequencerRow<T>::store_Row_Values()
{
    for (int c = 0; c < values.size(); c++)
    {
        bool myVal;
        // TODO: cell is float..
        myVal = (bool) get_CellValue(c);//myVal = parameter->get();
        
        cout <<  "store_Row_Values: c: " << c << " = " << myVal << endl;
        
        // store in row
        GRID_row_Values[c] = myVal;
    }
}

//------------------

// mouse handlers

template<class T>
void ofxSequencerRow<T>::mousePressed(int col, int x, int y)
{
    pMouse.set(x, y);
    pValue = values[col];
}

template<class T>
void ofxSequencerRow<T>::mouseDragged(int col, int y)
{
    
}

template<>
inline void ofxSequencerRow<float>::mouseDragged(int col, int y)
{
    values[col] = ofClamp(pValue - 0.01 * (y - pMouse.y) * (parameter->getMax() - parameter->getMin()), parameter->getMin(), parameter->getMax());
}

template<>
inline void ofxSequencerRow<int>::mouseDragged(int col, int y)
{
    values[col] = ofClamp(pValue - 0.01 * (y - pMouse.y) * (parameter->getMax() - parameter->getMin()), parameter->getMin(), parameter->getMax());
}

//------------------

// drawing functions

template<class T>
void ofxSequencerRow<T>::draw(int col, int cellWidth, int cellHeight)
{
    float rectMult = 1.0 / (parameter->getMax() - parameter->getMin());
    ofDrawRectangle(0, 0,
           cellWidth  * rectMult * (values[col] - parameter->getMin()),
           cellHeight * rectMult * (values[col] - parameter->getMin()));
}

template<>
inline void ofxSequencerRow<bool>::draw(int col, int cellWidth, int cellHeight)
{
    if (values[col])
    {
        ofDrawRectangle(0, 0, cellWidth, cellHeight);
    }
}

//------------------

// mouse handlers

template<class T>
void ofxSequencerRow<T>::mouseReleased(int col)
{
    if (values[col] > 0.5 * (parameter->getMin() + parameter->getMax())){
        values[col] = parameter->getMin();
    }
    else {
        values[col] = parameter->getMax();
    }
}

//------------------

template<>
inline void ofxSequencerRow<bool>::mouseReleased(int col)
{
    values[col] = 1.0 - values[col];
}

//------------------

// rowBase getters

template<class T> T ofxSequencerRowBase::getMin()
{
    return dynamic_cast<ofxSequencerRow<T>&>(*this).getMin();
}

template<class T> T ofxSequencerRowBase::getMax()
{
    return dynamic_cast<ofxSequencerRow<T>&>(*this).getMax();
}

template<class T> T ofxSequencerRowBase::getValue()
{
    return dynamic_cast<ofxSequencerRow<T>&>(*this).getValue();
}

//------------------

// class methods

class ofxSequencer
{
public:
    ofxSequencer();
    ~ofxSequencer();
    
    void setup(int cols, int beatsPerMinute=120, int beatsPerBar=4);
    void setBpm(int beatsPerMinute, int beatsPerBar=4);
    void setSmooth(bool smooth) {this->smooth = smooth;}
    
    int getBpm() {return beatsPerMinute;}
    bool getSmooth() {return smooth;}
    
    //--
    
    template<class T>
    void addRow(ofParameter<T> * parameter);

    //--

    void GRID_Refresh();
    
    //--
    
    void start();
    
    void advance();
    
    //TODO: added methods
    void stepBack();
    
    void stop();
    void reset();
    void randomize();
    
    //--
    
    void get_AllValues();
    
    void DEBUG_All_GRID();//show store GRID vector
    void REFRESH_All_GRID();//show store GRID vector
    
    //--
    
    void update();
    void draw();
    
    void setPosition(int x, int y, int width, int height);
    void setVisible(bool visible);
    void toggleVisible();
    void setMouseActive(bool active);
    
    //--
    
    template<class T>
    void setValue(int r, int c, T value) {
        ((ofxSequencerRow<T>*) rows[r])->setValue(c, value);
        toRedraw = true;
    }
    
    //--
    
    template<class T>
    bool get_Value(int r, int c) {
        bool myValue = ((ofxSequencerRow<T>*) rows[r])->get_Value(c);
        
        cout << "-- get_Value " << "r:" << r << " c:" << c << " get_Value:" << myValue;
        return myValue;
    }
    
    int getColumn() {return column;}
    vector<ofxSequencerRowBase*> & getRows() {return rows;}
    
    //--
    
    ofEvent<int> beatEvent;
    
    //--

    // made public..
    float cursor;
    int column;

    //--
    
    vector < vector <bool> > GRID_RowsByCols_values;//all cols in all rows
    
    //------------------
    
private:
    
    void play(void);
    void redraw();
    
    //-
    
    void mousePressed(ofMouseEventArgs &evt);
    void mouseDragged(ofMouseEventArgs &evt);
    void mouseReleased(ofMouseEventArgs &evt);
    
    //-
    
    vector<ofxSequencerRowBase*> rows;
    
    //-
    
    //    float cursor;//go to public breaks something..(?)
    //    int column;
    
    int beatsPerMinute;
    int bpmInterval;
    int bpmTime;
    bool smooth;
    ofxSequencerBpm bpm;

    //-
    
    int cols;
    int x, y, width, height;
    float cellWidth, cellHeight;
    
    ofPoint mCell;
    bool draggingCell;
    int draggingFrames;
    
    ofFbo fbo;
    bool toRedraw;
};

//------------------

template<class T>
void ofxSequencer::addRow(ofParameter<T> * parameter)
{
    ofxSequencerRow<T> *newRow = new ofxSequencerRow<T>(parameter, cols);
    rows.push_back(newRow);
    toRedraw = true;
    
    //--
    
    // erase all bools cols for any rows?
    for (int c = 0 ; c < cols; c++)
    {
        vector <bool> myBools;
        bool defState = false;
        myBools.push_back(defState);//define cell
        GRID_RowsByCols_values.push_back(myBools);//create row
    }
    
    //--
}

//------------------

