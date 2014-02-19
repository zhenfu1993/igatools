//-+--------------------------------------------------------------------
// This file is part of igatools, a general purpose Isogeometric analysis
// library. It was copied from the deal.II project where it is licensed
// under the LGPL (see http://www.dealii.org/).
// I has been modified by the igatools authors to fit the igatools framework.
//-+--------------------------------------------------------------------

#include <igatools/base/logstream.h>
#include <sys/resource.h>
#include <unistd.h>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>


IGA_NAMESPACE_OPEN

LogStream::LogStream(const std::string head)
    :
    std_out(&std::cerr), file(0),
    std_depth(10000), file_depth(10000),
    print_utime(false), diff_utime(false),
    last_time(0.), double_threshold(0.), long_double_threshold(0.), float_threshold(0.),
    offset(0), old_cerr(0),
    format_flags_(std::ios::showpoint | std::ios::left),
    precision_(6)
{
    prefixes.push(head);
    std_out->setf(format_flags_);
    std_out->precision(precision_);
//#if defined(HAVE_UNISTD_H) && defined(HAVE_TIMES)
    reference_time_val = 1./sysconf(_SC_CLK_TCK) * times(&reference_tms);
//#endif
}


LogStream::~LogStream()
{
    // if there was anything left in
    // the stream that is current to
    // this thread, make sure we flush
    // it before it gets lost
    {
        //const unsigned int id = Threads::this_thread_id();
        const unsigned int id =0;
        if ((outstreams.find(id) != outstreams.end())
            &&
            (*outstreams[id] != 0)
            &&
            (outstreams[id]->str().length() > 0))
        {
            // except the situation is
            // not quite that simple. if
            // this object is the
            // 'deallog' object, then it
            // is destroyed upon exit of
            // the program. since it's
            // defined in a shared
            // library that depends on
            // libstdc++.so, destruction
            // happens before destruction
            // of std::cout/cerr, but
            // after all file variables
            // defined in user programs
            // have been destroyed. in
            // other words, if we get
            // here and the object being
            // destroyed is 'deallog' and
            // if 'deallog' is associated
            // with a file stream, then
            // we're in trouble: we'll
            // try to write to a file
            // that doesn't exist any
            // more, and we're likely
            // going to crash (this is
            // tested by
            // base/log_crash_01). rather
            // than letting it come to
            // this, print a message to
            // the screen (note that we
            // can't issue an assertion
            // here either since Assert
            // may want to write to
            // 'deallog' itself, and
            // AssertThrow will throw an
            // exception that can't be
            // caught)
//        if ((this == &deallog) && file)
//          std::cerr << ("You still have content that was written to 'deallog' "
//                        "but not flushed to the screen or a file while the "
//                        "program is being terminated. This would lead to a "
//                        "segmentation fault. Make sure you flush the "
//                        "content of the 'deallog' object using 'std::endl' "
//                        "before the end of the program.")
//                    << std::endl;
//        else
            *this << std::endl;
        }
    }

    if (old_cerr)
        std::cerr.rdbuf(old_cerr);

   
}


void
LogStream::test_mode(bool on)
{

    if (on)
    {
        long_double_threshold = 1.e-10;
        double_threshold = 1.e-10;
        float_threshold = 1.e-7;
        offset = 1.e-7;
    }
    else
    {
        long_double_threshold = 0.;
        double_threshold = 0.;
        float_threshold = 0.;
        offset = 0.;
    }
}


LogStream &
LogStream::operator<< (std::ostream& (*p)(std::ostream &))
{
    // do the work that is common to
    // the operator<< functions
    print(p);

    // next check whether this is the
    // <tt>endl</tt> manipulator, and if so
    // set a flag
    std::ostream & (* const p_endl)(std::ostream &) = &std::endl;
    if (p == p_endl)
    {
        //Threads::ThreadMutex::ScopedLock lock(write_lock);
        print_line_head();
        std::ostringstream &stream = get_stream();
        if (prefixes.size() <= std_depth)
            *std_out << stream.str();

        if (file && (prefixes.size() <= file_depth))
            *file << stream.str() << std::flush;

        // Start a new string
        stream.str("");
    }
    return *this;
}


std::ostringstream &
LogStream::get_stream()
{
//TODO: use a ThreadLocalStorage object here
//  Threads::ThreadMutex::ScopedLock lock(log_lock);
// const unsigned int id = Threads::this_thread_id();
    const unsigned int id = 0;
    // if necessary allocate a stream object
    if (outstreams.find(id) == outstreams.end())
    {
        outstreams[id].reset(new std::ostringstream());
        outstreams[id]->setf(format_flags_);
        outstreams[id]->precision(precision_);
    }
    return *outstreams[id];
}



void
LogStream::attach(std::ostream &o)
{
    //Threads::ThreadMutex::ScopedLock lock(log_lock);
    file = &o;
    o.setf(format_flags_);
//  o << dealjobid();
}


void LogStream::detach()
{
    //Threads::ThreadMutex::ScopedLock lock(log_lock);
    file = 0;
}


void LogStream::log_cerr()
{
    //Threads::ThreadMutex::ScopedLock lock(log_lock);
    if (old_cerr == 0)
    {
        old_cerr = std::cerr.rdbuf(file->rdbuf());
    }
    else
    {
        std::cerr.rdbuf(old_cerr);
        old_cerr = 0;
    }
}


std::ostream &
LogStream::get_console()
{
    return *std_out;
}


std::ostream &
LogStream::get_file_stream()
{
    Assert(file, ExcNoFileStreamGiven());
    return *file;
}


bool
LogStream::has_file() const
{
    return (file != 0);
}


const std::string &
LogStream::get_prefix() const
{
    return prefixes.top();
}


unsigned int
LogStream::depth_console(const unsigned int n)
{
    //Threads::ThreadMutex::ScopedLock lock(log_lock);
    const unsigned int h = std_depth;
    std_depth = n;
    return h;
}


unsigned int
LogStream::depth_file(const unsigned int n)
{
    //Threads::ThreadMutex::ScopedLock lock(log_lock);
    const unsigned int h = file_depth;
    file_depth = n;
    return h;
}


void
LogStream::threshold_double(const double t)
{
    //Threads::ThreadMutex::ScopedLock lock(log_lock);
    double_threshold = t;
}


void
LogStream::threshold_float(const float t)
{
    //Threads::ThreadMutex::ScopedLock lock(log_lock);
    float_threshold = t;
}

void
LogStream::threshold_long_double(const long double t)
{
    //Threads::ThreadMutex::ScopedLock lock(log_lock);
    long_double_threshold = t;
}


bool
LogStream::log_execution_time(const bool flag)
{
    //Threads::ThreadMutex::ScopedLock lock(log_lock);
    const bool h = print_utime;
    print_utime = flag;
    return h;
}


bool
LogStream::log_time_differences(const bool flag)
{
    //Threads::ThreadMutex::ScopedLock lock(log_lock);
    const bool h = diff_utime;
    diff_utime = flag;
    return h;
}


bool
LogStream::log_thread_id(const bool flag)
{
    //Threads::ThreadMutex::ScopedLock lock(log_lock);
    const bool h = print_thread_id;
    print_thread_id = flag;
    return h;
}


void
LogStream::print_line_head()
{
//#ifdef HAVE_SYS_RESOURCE_H
    rusage usage;
    double utime = 0.;
    if (print_utime)
    {
        getrusage(RUSAGE_SELF, &usage);
        utime = usage.ru_utime.tv_sec + 1.e-6 * usage.ru_utime.tv_usec;
        if (diff_utime)
        {
            double diff = utime - last_time;
            last_time = utime;
            utime = diff;
        }
    }
//#else
////TODO[BG]: Do something useful here
//  double utime = 0.;
//#endif



    const std::string &head = get_prefix();
// const unsigned int thread = Threads::this_thread_id();
    const unsigned int thread = 0;
    if (prefixes.size() <= std_depth)
    {
        if (print_utime)
        {
            int p = std_out->width(5);
            *std_out << utime << ':';

            std_out->width(p);
        }
        if (print_thread_id)
            *std_out << '[' << thread << ']';

        *std_out <<  head ;
    }

    if (file && (prefixes.size() <= file_depth))
    {
        if (print_utime)
        {
            int p = file->width(6);
            *file << utime << ':';

            file->width(p);
        }
        if (print_thread_id)
            *file << '[' << thread << ']';

        *file << head ;
    }
}


void
LogStream::timestamp()
{
    struct tms current_tms;
//#if defined(HAVE_UNISTD_H) && defined(HAVE_TIMES)
    const clock_t tick = sysconf(_SC_CLK_TCK);
    const double time = 1./tick * times(&current_tms);
//#else
//  const double time = 0.;
//  const unsigned int tick = 100;
//#endif
    (*this) << "Wall: " << time - reference_time_val
            << " User: " << 1./tick * (current_tms.tms_utime - reference_tms.tms_utime)
            << " System: " << 1./tick * (current_tms.tms_stime - reference_tms.tms_stime)
            << " Child-User: " << 1./tick * (current_tms.tms_cutime - reference_tms.tms_cutime)
            << " Child-System: " << 1./tick * (current_tms.tms_cstime - reference_tms.tms_cstime)
            << std::endl;
}


std::size_t
LogStream::memory_consumption() const
{
    std::size_t mem = sizeof(*this);
//  // to determine size of stack
//  // elements, we have to copy the
//  // stack since we can't access
//  // elements from further below
//  std::stack<std::string> tmp = prefixes;
//  while (tmp.empty() == false)
//    {
//      mem += MemoryConsumption::memory_consumption (tmp.top());
//      tmp.pop ();
//    }

    return mem;
}




IGA_NAMESPACE_CLOSE