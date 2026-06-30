import struct, sys, cv2, uuid
import numpy as np


class BaseConfig:
    METADATA_FORMAT: str = (
        "<3sBBHHI"  # magic word; version; fps; width; height; frames count.
    )
    CAV_VER = 0  # format version

    TARGET_WIDTH = 80  # fixed width of CAV file
    TARGET_COLORS = 10  # how much characters we have to display video


def main():
    Config = BaseConfig()

    if len(sys.argv) < 3:
        raise TypeError("Please specify original file path and new CAV file path!")

    from_path = sys.argv[1]
    to_path = f"{sys.argv[2]}/{uuid.uuid4()}.cav"

    video = cv2.VideoCapture(from_path)
    if not video.isOpened:
        raise ValueError("Can't open the video!")

    fps = int(video.get(cv2.CAP_PROP_FPS))
    frame_count = int(video.get(cv2.CAP_PROP_FRAME_COUNT))
    orig_width = int(video.get(cv2.CAP_PROP_FRAME_WIDTH))
    orig_height = int(video.get(cv2.CAP_PROP_FRAME_HEIGHT))

    width = Config.TARGET_WIDTH
    ratio = orig_height / orig_width
    height = int(width * ratio * 0.5)

    cav_file = open(to_path, "wb")
    cav_file.write(
        struct.pack(
            Config.METADATA_FORMAT,
            b"CAV",
            Config.CAV_VER,
            fps,
            width,
            height,
            frame_count,
        )
    )

    for i in range(frame_count):
        success, frame = video.read()
        if not success:
            raise Exception("Something went wrong while reading the video!")

        frame = cv2.cvtColor(cv2.resize(frame, (width, height)), cv2.COLOR_BGR2GRAY)
        flat_frame = (frame.flatten() / 255 * (Config.TARGET_COLORS - 1)).astype(
            np.uint8
        )

        cav_file.write(flat_frame)

    cav_file.close()
    print(f"File converted at {to_path}")


if __name__ == "__main__":
    main()
