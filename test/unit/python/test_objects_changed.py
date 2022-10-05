# -*- coding: utf-8 -*-

from pxr import Usd, Tf, Sdf
from usd_notice_framework import Broker, BrokerNotice


def test_resynced_object():
    """Check whether object has been resynced."""
    stage = Usd.Stage.CreateInMemory()
    Broker.Create(stage)

    stage.DefinePrim("/Foo")

    received = []

    def _validate(notice, stage):
        """Validate notice received."""
        assert notice.ResyncedObject(stage.GetPrimAtPath("/Foo")) is False
        assert notice.ResyncedObject(stage.GetPrimAtPath("/Bar")) is True
        assert notice.AffectedObject(stage.GetPrimAtPath("/Foo")) is False
        assert notice.AffectedObject(stage.GetPrimAtPath("/Bar")) is True
        received.append(notice)

    key = Tf.Notice.Register(BrokerNotice.ObjectsChanged, _validate, stage)

    stage.DefinePrim("/Bar")

    # Ensure that one notice was received.
    assert len(received) == 1


def test_changed_info_only():
    """Check whether only info from object has changed."""
    stage = Usd.Stage.CreateInMemory()
    Broker.Create(stage)

    stage.DefinePrim("/Foo")
    prim = stage.DefinePrim("/Bar")

    received = []

    def _validate(notice, stage):
        """Validate notice received."""
        assert notice.ChangedInfoOnly(stage.GetPrimAtPath("/Foo")) is False
        assert notice.ChangedInfoOnly(stage.GetPrimAtPath("/Bar")) is True
        assert notice.AffectedObject(stage.GetPrimAtPath("/Foo")) is False
        assert notice.AffectedObject(stage.GetPrimAtPath("/Bar")) is True
        received.append(notice)

    key = Tf.Notice.Register(BrokerNotice.ObjectsChanged, _validate, stage)

    prim.SetMetadata("comment", "This is a test")

    # Ensure that one notice was received.
    assert len(received) == 1


def test_get_resynced_paths():
    """Ensure that expected resynced paths are returned."""
    stage = Usd.Stage.CreateInMemory()
    Broker.Create(stage)

    received = []

    def _validate(notice, stage):
        """Validate notice received."""
        assert notice.GetResyncedPaths() == [Sdf.Path("/Foo")]
        received.append(notice)

    key = Tf.Notice.Register(BrokerNotice.ObjectsChanged, _validate, stage)

    stage.DefinePrim("/Foo")

    # Ensure that one notice was received.
    assert len(received) == 1


def test_get_changed_info_only_paths():
    """Ensure that expected paths with non-resyncable info are returned."""
    stage = Usd.Stage.CreateInMemory()
    Broker.Create(stage)

    prim = stage.DefinePrim("/Foo")

    received = []

    def _validate(notice, stage):
        """Validate notice received."""
        assert notice.GetChangedInfoOnlyPaths() == [Sdf.Path("/Foo")]
        received.append(notice)

    key = Tf.Notice.Register(BrokerNotice.ObjectsChanged, _validate, stage)

    prim.SetMetadata("comment", "This is a test")

    # Ensure that one notice was received.
    assert len(received) == 1

def test_get_changed_fields():
    """Ensure that expected set of fields are returned."""
    stage = Usd.Stage.CreateInMemory()
    Broker.Create(stage)

    prim = stage.DefinePrim("/Foo")

    received = []

    def _validate(notice, stage):
        """Validate notice received."""
        assert notice.GetChangedFields(prim) == ["comment"]
        assert notice.GetChangedFields(Sdf.Path("/Foo")) == ["comment"]
        assert notice.GetChangedFields(Sdf.Path("/Incorrect")) == []
        received.append(notice)

    key = Tf.Notice.Register(BrokerNotice.ObjectsChanged, _validate, stage)

    prim.SetMetadata("comment", "This is a test")

    # Ensure that one notice was received.
    assert len(received) == 1

def test_has_changed_fields():
    """Check whether path or prim have changed fields."""
    stage = Usd.Stage.CreateInMemory()
    Broker.Create(stage)

    prim = stage.DefinePrim("/Foo")

    received = []

    def _validate(notice, stage):
        """Validate notice received."""
        assert notice.HasChangedFields(prim) is True
        assert notice.HasChangedFields(Sdf.Path("/Foo")) is True
        assert notice.HasChangedFields(Sdf.Path("/Incorrect")) is False
        received.append(notice)

    key = Tf.Notice.Register(BrokerNotice.ObjectsChanged, _validate, stage)

    prim.SetMetadata("comment", "This is a test")

    # Ensure that one notice was received.
    assert len(received) == 1
