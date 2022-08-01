#include "KrimzLib.h"


int main(int argc, const char** argv) {
	std::string filePath;
	if (argc == 2) {
		filePath = argv[1];
	}
	else {
		kl::print<false>("Video file path: ");
		std::getline(std::cin, filePath);
	}

	kl::video video(filePath);
	kl::console::error(!video, "Failed to open video file \"" + filePath + "\"");

	kl::console::clear();
	kl::print<false>("Resize the console and press enter..");
	kl::get();

	const kl::int2 consoleSize = kl::console::size() - kl::int2(0, 1);
	kl::console::cursor(false);

	const uint64 frameCount = video.count();
	std::vector<std::string> asciiFrames;
	asciiFrames.reserve(frameCount);

	kl::image videoFrame;
	kl::console::clear();
	while (video.get(videoFrame)) {
		asciiFrames.push_back(videoFrame.ascii(consoleSize));
		kl::console::cursor(kl::uint2());
		kl::print<false>("Processed: ", asciiFrames.size(), "/", frameCount);
	}

	kl::timer timer;
	kl::console::clear();
	float toWait = 1.0f / video.fps();

	while (true) {
		for (uint64 i = 0; i < asciiFrames.size(); i++) {
			timer.reset();
			kl::console::dump(asciiFrames[i]);
			kl::console::title(kl::format(i + 1, "/", asciiFrames.size()));
			while (timer.elapsed() < toWait);
		}
	}
}
