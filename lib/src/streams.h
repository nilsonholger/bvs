#ifndef BVS_STREAMS_H
#define BVS_STREAMS_H

#include <iostream>



/** BVS namespace, contains all library stuff. */
namespace BVS
{
	/** A streambuffer Duplicator. */
	class BufferDup: public std::streambuf
	{
		public:
			/** Construct streambuffer that duplicates to two streambuffers.
			 * @param[in] out First streambuffer.
			 * @param[in] copy Second streambuffer.
			 */
			BufferDup(std::streambuf* out, std::streambuf* copy) : out{out} , copy{copy} {}

		protected:
			/** Since there is no actual buffer, every character "overflows" and gets forwarded.
			 * @param[in] c Character to check.
			 * @return Character or EOF.
			 */
			virtual int overflow(int c)
			{
				if (c == EOF)
				{
					return !EOF;
				}
				else
				{
					return out->sputc(c) == EOF || copy->sputc(c) == EOF ? EOF : c;
				}
			}
			/** Sync both streams.
			 * @return Sync status.
			 */
			virtual int sync()
			{
				return out->pubsync() == 0 && copy->pubsync() == 0 ? 0 : -1;
			}

		private:
			std::streambuf* out; /**< The first of two output streambuffers. */
			std::streambuf* copy; /**< The second of two output streambuffers. */
			BufferDup(const BufferDup&) = delete; /**< -Weffc++ */
			BufferDup& operator=(const BufferDup&) = delete; /**< -Weffc++ */
	};



	/** An ostream Duplicator. */
	class StreamDup : public std::ostream
	{
		public:
			/** Construct ostream that duplicates its input into to other ostreams.
			 * @param[in,out] one Stream one where output is redirected to.
			 * @param[in,out] two Stream two where output is redirected to.
			 */
			StreamDup(std::ostream& one, std::ostream& two) : std::ostream{&bufferDup}, bufferDup{one.rdbuf(), two.rdbuf()} {}

		private:
			BufferDup bufferDup; /**< A streambuffer doing the actual work of duplicating. */
	};



	/** A Stream pointing to nirvana. */
	class NullStream : public std::ostream
	{
		public:
			/** Construct an ostream that collects input, but never outputs.
			 * This is done by initializing the underlying ostream and ios with 0.
			 */
			NullStream(): std::ios{0}, std::ostream{0}
		{
			setstate(std::ios_base::badbit);
		}
	};
} // namespace BVS



#endif //BVS_STREAMS_H

