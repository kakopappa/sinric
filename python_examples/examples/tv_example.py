from loguru import logger

from sinric import Sinric

apiKey = ""


def setPowerState(did, value):
    logger.info("{} {}".format(did, value))


def SetMute(did, value):
    logger.info("{} {}".format(did, value))


def AdjustVolume(did, volume, d_volume):
    logger.info("{} {} {}".format(did, volume, d_volume))


def ChangeChannel(did, channel, channel_name):
    logger.info("{} {} {}".format(did, channel, channel_name))


def SkipChannels(did, channel_count):
    logger.info("{} {}".format(did, channel_count))


def Previous_Play(did, value):
    logger.info("{} {} ".format(did, value))


def SelectInput(did, value):
    logger.info("{} {} ".format(did, value))


if __name__ == "__main__":
    sinric = Sinric(
        apiKey,
        callbacks={
            "setPowerState": setPowerState,
            "SetMute": SetMute,
            "AdjustVolume": AdjustVolume,
            "ChangeChannel": ChangeChannel,
            "SkipChannels": SkipChannels,
            "Previous": Previous_Play,
            "SelectInput": SelectInput,
        },
    )
    sinric.handle()
