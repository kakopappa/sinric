from sinric import Sinric
import logging

apiKey = ''


def setPowerState(did, value):
    print(did, value)


def SetMute(did, value):
    print(did, value)


def AdjustVolume(did, volume, d_volume):
    print(did, volume, d_volume)


def ChangeChannel(did, channel, channel_name):
    print(did, channel, channel_name)


def SkipChannels(did, channel_count):
    print(did, channel_count)


def Previous_Play(did, value):
    print(did, value)


def SelectInput(did, value):
    print(did, value)


if __name__ == '__main__':
    logging.basicConfig(filename='myapp.log', level=logging.INFO)
    sinric = Sinric(apiKey, callbacks={
        'setPowerState': setPowerState,
        'SetMute': SetMute,
        'AdjustVolume': AdjustVolume,
        'ChangeChannel': ChangeChannel,
        'SkipChannels': SkipChannels,
        'Previous': Previous_Play,
        'SelectInput': SelectInput

    })
    sinric.handle()
