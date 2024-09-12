from pxr import Usd
import unf


def test_inspector_create():
    """Create a single inspector per stage."""
    stage = Usd.Stage.CreateInMemory()
    inspector1 = unf.Inspector.Create(stage)
    assert unf.Inspector.GetNumInspectors() == 1

    inspector2 = unf.Inspector.Create(stage)
    assert unf.Inspector.GetNumInspectors() == 1
    assert inspector1 == inspector2

    otherStage = Usd.Stage.CreateInMemory()
    inspector3 = unf.Inspector.Create(otherStage)
    assert unf.Inspector.GetNumInspectors() == 2
    assert inspector1 != inspector3

def test_inspector_stage_expired():
    """Clean inspector when stage is expired."""
    stage1 = Usd.Stage.CreateInMemory()
    inspector1 = unf.Inspector.Create(stage1)

    # Stage is destroyed, but inspector reference is kept in registry
    del stage1
    assert inspector1.expired is False
    assert unf.Inspector.GetNumInspectors() == 1

    # Registry reference is removed when a inspector is added
    stage2 = Usd.Stage.CreateInMemory()
    inspector2 = unf.Inspector.Create(stage2)
    assert inspector1.expired is True
    assert inspector2.expired is False
    assert unf.Inspector.GetNumInspectors() == 1

def test_inspector_reset():
    """Explicitly reset inspector."""
    stage1 = Usd.Stage.CreateInMemory()
    inspector1 = unf.Inspector.Create(stage1)

    stage2 = Usd.Stage.CreateInMemory()
    inspector2 = unf.Inspector.Create(stage2)

    # Explicitly reset one inspector
    inspector1.Reset()
    assert inspector1.expired is True
    assert inspector2.expired is False

def test_inspector_reset_all():
    """Explicitly reset all inspectors."""
    stage1 = Usd.Stage.CreateInMemory()
    inspector1 = unf.Inspector.Create(stage1)

    stage2 = Usd.Stage.CreateInMemory()
    inspector2 = unf.Inspector.Create(stage2)

    # Explicitly reset one inspector
    unf.Inspector.ResetAll()
    assert inspector1.expired is True
    assert inspector2.expired is True
