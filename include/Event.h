#ifndef EVENT_H
#define EVENT_H


class Event {
    public:
        Event () {
             stime_ = 0.0;
            rtime_ = 0.0;
        }

           inline int& eventType () { return (eventType_); }
        inline double& expire () { return (rtime_); }
        inline double& start () { return (stime_); }

        void activate (double t);
        void cancel ();
        virtual void handle () = 0;

        Event* next_;

    private:
        int eventType_;    // represented by event id
        double stime_;
        double rtime_;
        int status_;
};


#endif
