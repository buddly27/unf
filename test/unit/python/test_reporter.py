from pxr import Usd, Tf
import unf
import unf

import unf_test


def test_reporter_notice():
    """Ensure that reporter's notices are received."""
    stage = Usd.Stage.CreateInMemory()
    unf.ReporterManager.Create(stage)
    unf.Broker.Create(stage)

    received = []

    def _update(notice, _):
        """process notices."""
        received.append(type(notice).__name__)

    key1 = Tf.Notice.Register(
        unf_test.TestReporterNotice1, 
        _update, stage
    )
    
    key2 = Tf.Notice.Register(
        unf_test.TestReporterNotice2, 
        _update, stage
    )

    key3 = Tf.Notice.Register(
        unf_test.ChildReporterNotice, 
        _update, stage
    )

    stage.DefinePrim("/Foo")

    assert sorted(received) == [
        "ChildReporterNotice",
        "TestReporterNotice1",
        "TestReporterNotice2",
    ]
